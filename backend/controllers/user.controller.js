import User from "../models/user.model.js";
import jwt from "jsonwebtoken";
import * as EmailValidator from "email-validator";
import bcrypt from "bcryptjs";
import cookieParser from "cookie-parser";

const getUser = async (req, res) => {
    try {
        const token = req.token;
        res.status(200).json({message: "User authorized", token});
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
}


const signup = async (req, res) => {
    const {email, name, password, role} = req.body;
    if(!EmailValidator.validate(email)){
        return res.status(400).json({message: "Invalid email"});
    }
    if(!password){
        return res.status(400).json({message: "Password is required"});
    }
    if(role !== "admin" && role !== "student"){
        return res.status(400).json({message: "Role is required"});
    }
    if(password.length < 6){
        return res.status(400).json({message: "Password must be at least 6 characters long"});
    }
    
    // const hashedPassword = await bcrypt.hash(password, 10);
    // const user = new User({email, name, password: hashedPassword, role});
    // await user.save();

    const accessToken = jwt.sign({email}, process.env.ACCESS_TOKEN, {expiresIn: "10d"})
    req.token = accessToken;


    return res.status(200).json({accessToken});
}

export { getUser, signup };