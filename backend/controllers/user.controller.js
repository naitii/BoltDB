import User from "../models/user.model.js";
import jwt from "jsonwebtoken";
import * as EmailValidator from "email-validator";
import bcrypt from "bcryptjs";


const getUser = async (req, res) => {
    try {
        const email = req.user.email;
        const user = await User.findOne({email});
        if(!user){
            return res.status(404).json({message: "User not found"});
        }
        res.status(200).json(user);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
}

const setRole = async (req, res) => {
    try {
        const {role} = req.body;
        const email = req.user.email
        const user = await User.findOne({email});
        if(!user){
            return res.status(404).json({message: "User not found"});
        }
        // if(user.role !== "none"){
        //     return res.status(400).json({message: "Role can only be set once"});
        // }
        if(role !== "admin" && role !== "student"){
            return res.status(400).json({message: "Role can only be admin or student"});
        }
        user.role = role;
        await user.save();
        return res.status(200).json(user);
    }
    catch (error) {
        res.status(500).json({ message: error.message });
    }
}

export { getUser, setRole };