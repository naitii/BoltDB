import User from "../models/user.model.js";
import jwt from "jsonwebtoken";
import * as EmailValidator from "email-validator";
import bcrypt from "bcryptjs";


const getUser = async (req, res) => {
    try {
        const token =
          req.headers.cookie.substring(6) ||
          req.headers.authorization?.split(" ")[1];
        if(!token) {
            return res.status(401).json({message: "Unauthorized"});
        }
        const decoded = jwt.verify(token, process.env.ACCESS_TOKEN);
        const user = await User.findOne({email: decoded.email});
        if(!user){
            return res.status(404).json({message: "User not found"});
        }
        res.status(200).json(user);
        
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
}


// const signup = async (req, res) => {
//     const {email, name, password, role} = req.body;
//     if(!EmailValidator.validate(email)){
//         return res.status(400).json({message: "Invalid email"});
//     }
//     if(!password){
//         return res.status(400).json({message: "Password is required"});
//     }
//     if(role !== "admin" && role !== "student"){
//         return res.status(400).json({message: "Role is required"});
//     }
//     if(password.length < 6){
//         return res.status(400).json({message: "Password must be at least 6 characters long"});
//     }
//     const existingUser = await User.find({email});
//     if(existingUser.length > 0){        
//         return res.status(400).json({message: "User already exists. Try Logging in."});
//     }
    
//     const hashedPassword = await bcrypt.hash(password, 10);
//     const user = new User({email, name, password: hashedPassword, role});
//     await user.save();

//     const accessToken = jwt.sign({email}, process.env.ACCESS_TOKEN, {expiresIn: "10d"})
//     res.cookie("token", accessToken, {httpOnly: true, secure: true, sameSite: "none"}); 
//     return res.status(200).send(user);
// }

// const login = async (req, res) => {
//     try {
//         const {email, password} = req.body;
//         if(!EmailValidator.validate(email)){
//             return res.status(400).json({message: "Invalid email"});
//         }
//         if(!password){
//             return res.status(400).json({message: "Password is required"});
//         }
//         const user = await User.findOne({email});
//         if(!user){
//             return res.status(404).json({message: "User not found! Try signing up."});
//         }
//         const isMatch = await bcrypt.compare(password, user.password);
//         if(!isMatch){
//             return res.status(400).json({message: "Invalid credentials"});
//         }
//         const accessToken = jwt.sign({email}, process.env.ACCESS_TOKEN, {expiresIn: "10d"})
//         res.cookie("token", accessToken, {httpOnly: true, secure: true, sameSite: "none"});
//         return res.status(200).send(user);
//     } catch (error) {
//         res.status(500).json({ message: error.message });
//     }
// }

const setRole = async (req, res) => {
    try {
        const {email, role} = req.body;
        const user = await User.findOne({email});
        if(!user){
            return res.status(404).json({message: "User not found"});
        }
        if(user.role !== "none"){
            return res.status(400).json({message: "Role can only be set to none"});
        }
    }
    catch (error) {
        res.status(500).json({ message: error.message });
    }
}

export { getUser, setRole };