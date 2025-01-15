import User from "../models/user.model.js";
import jwt from "jsonwebtoken";
import Test from "../models/test.model.js";

const createTest = async (req, res) => {
    try{
        const decode = jwt.verify(
          req.headers.cookie.substring(6) ||
            req.headers.authorization?.split(" ")[1],
          process.env.ACCESS_TOKEN
        );
        const user = await User.findOne({email: decode.email});
        if(!user){
            return res.status(401).json({message: "Unauthorized"});
        }
        if(user.role !== "admin"){
            return res.status(403).json({message: "Forbidden"});
        }
        const { name, description, duration, testDate, startTime } = req.body;
        
        if(!name || !description || !testDate || !startTime){
            return res.status(400).json({message: "All fields are required"});
        }
        // console.log("here");

        if(duration <= 0){
            return res.status(400).json({message: "Duration cannot be zero or negative"});
        }
        const [day, month, year] = testDate.split(":");
        const formattedDate = `${year}-${month}-${day}`;
        if(new Date(formattedDate) < new Date().toISOString().substring(0, 10) ){
            return res.status(400).json({message: "Test Date cannot be in the past"});
        }
        if(startTime.length !== 5 || startTime[2] !== ":"){
            return res.status(400).json({message: "Invalid Start Time"});
        }
        if(parseInt(startTime.substring(0, 2)) > 23 || parseInt(startTime.substring(3)) > 59){
            return res.status(400).json({message: "Invalid Start Time"});
        }
        if(parseInt(startTime.substring(0, 2)) < new Date().getHours() && formattedDate === new Date().toISOString().substring(0, 10)){
            return res.status(400).json({message: "Start Time cannot be in the past"});
        }

        const newTest = await Test.create({
                            name,
                            description,
                            duration,
                            testDate,
                            startTime,
                            createdBy: user._id
                        });
        await newTest.save();
        const tests = user.createdTests;
        tests.push(newTest);
        user.createdTests = tests;
        await user.save();

        res.status(200).json(newTest);
    }catch(err){
        res.status(500).json({message: "Internal Server Error"});
    }
};

const deleteTest = async (req, res) => {
    try{
        const decode = jwt.verify(
          req.headers.cookie.substring(6) ||
            req.headers.authorization?.split(" ")[1],
          process.env.ACCESS_TOKEN
        );
        const user = await User.findOne({email: decode.email});
        if(!user){
            return res.status(401).json({message: "Unauthorized"});
        }
        if(user.role !== "admin"){
            return res.status(403).json({message: "Forbidden"});
        }
        const test = await Test.findById(req.body.id);
        if(!test){
            return res.status(404).json({message: "Test not found"});
        }
        await Test.findByIdAndDelete(req.body.id);
        const tests = user.createdTests;
        const index = tests.indexOf(req.body.id);
        if(index >-1){
            tests.splice(index, 1);
            user.createdTests = tests;
            await user.save();
        }
        res.status(200).json({message: "Test Deleted"});
    }catch(err){
        res.status(500).json({message: "Internal Server Error"});
    }
};

const getTests = async (req, res) => {
    try{
        const tests = await Test.find({_id: req.body.id});
        res.status(200).json(tests);
    }catch(err){
        res.status(500).json({message: "Internal Server Error"});
    }
}

export { createTest, deleteTest, getTests };