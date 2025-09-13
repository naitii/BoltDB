import jwt from "jsonwebtoken";
import User from "../models/user.model.js";
import Question from "../models/question.model.js";
import Test from "../models/test.model.js";

const createQuestion = async (req, res) => {
    try {
        const decode = jwt.verify(
          req.headers.cookie.substring(6) ||
            req.headers.authorization?.split(" ")[1],
          process.env.ACCESS_TOKEN
        );
        const user = await User.findOne({ email: decode.email });

        if (!user) {
            return res.status(401).json({ message: "Unauthorized" });
        }
        if (user.role !== "admin") {
            return res.status(403).json({ message: "Forbidden" });
        }
        const {
          text,
          type,
          image,
          subject,
          topic,
          positiveMarks,
          negativeMarks,
          testId,
        } = req.body;

        if (!text || !type) {
            return res.status(400).json({ message: "Question Text and it's type is required!" });
        }
        let question;
        if(type === "Multiple Choice" || type === "Multiple Correct") {
            const { options, correctOptions } = req.body;
            if (!options || !correctOptions) {
                return res.status(400).json({ message: "Options and Correct Options are Required" });
            }

            question = await Question.create({
              text,
              type,
              image,
              subject,
              topic,
              positiveMarks,
              negativeMarks,
              options,
              correctOptions,
              testId,
            });
        }

        if(type === "Integer Type") {
            const { integerAnswer } = req.body;
            if (!integerAnswer || !Number.isInteger(integerAnswer)) {
                return res.status(400).json({ message: "Integer Answer is Required" });
            }

            question = await Question.create({
              text,
              type,
              image,
              subject,
              topic,
              positiveMarks,
              negativeMarks,
              integerAnswer,
              testId,
            });
        }

        if(type === "Numerical Type") {
            const { numericalAnswer } = req.body;
            if (!numericalAnswer || !numericalAnswer.correctAnswer || !numericalAnswer.tolerance) {
                return res.status(400).json({ message: "Numerical Answer and Tolerance is Required" });
            }
            question = await Question.create({
              text,
              type,
              image,
              subject,
              topic,
              positiveMarks,
              negativeMarks,
              numericalAnswer,
              testId,
            });
        }
        await question.save();
        const test = await Test.findById(testId);
        const questions = test.questions;
        questions.push(question);
        test.questions = questions;
        await test.save();
        res.status(200).json(question);

    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

const editQuestion = async (req,res) => {
    try {
        const decode = jwt.verify(
          req.headers.cookie.substring(6) ||
            req.headers.authorization?.split(" ")[1],
          process.env.ACCESS_TOKEN
        );
        const user = await User.findOne({ email: decode.email });
        if(!user || !user.role === "admin") {
            return res.status(401).json({message: "Unauthorized"});
        }
        const question = await Question.findOne({ _id: req.body.id });
        if(!question) {
            return res.status(404).json({message: "Question not found"});
        }
        
        const data = req.body;
        if (data.text) {
          question.text = data.text;
        }
        if (data.type) {
          question.type = data.type;
        }
        if (data.image) {
          question.image = data.image;
        }
        if (data.subject) {
          question.subject = data.subject;
        }
        if (data.topic) {
          question.topic = data.topic;
        }
        if (data.positiveMarks) {
          question.positiveMarks = data.positiveMarks;
        }
        if (data.negativeMarks) {
          question.negativeMarks = data.negativeMarks;
        }
        if (data.options) {
          question.options = data.options;
        }
        if (data.correctOptions) {
          question.correctOptions = data.correctOptions;
        }
        if (data.integerAnswer) {
          question.integerAnswer = data.integerAnswer;
        }
        if (data.numericalAnswer) {
          question.numericalAnswer = data.numericalAnswer;
        }
        await question.save();
        res.status(200).json(question);

    } catch (error) {
        res.status(500).json({ message: error.message });
    }
}

const deleteQuestion = async (req, res) => {
    try {
        const decode = jwt.verify(
          req.headers.cookie.substring(6) ||
            req.headers.authorization?.split(" ")[1],
          process.env.ACCESS_TOKEN
        );
        const user = await User.findOne({ email: decode.email });
        if (!user || !user.role === "admin") {
          return res.status(401).json({ message: "Unauthorized" });
        }
        const question = await Question.findOne({ _id: req.body.id });
        if (!question) {
          return res.status(404).json({ message: "Question not found" });
        }
        const test = await Test.findById(question.testId);
        
        if(test.createdBy.toString() !== user._id.toString()) {
            return res.status(403).json({message: "Forbidden"});
        }
        await Question.findByIdAndDelete(req.body.id);
        const questions = test.questions;
        const index = questions.indexOf(req.body.id);
        if (index > -1) {
          questions.splice(index, 1);
          test.questions = questions;
          await test.save();
        }
        res.status(200).json({ message: "Question Deleted" });
        
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
}


export { createQuestion, editQuestion, deleteQuestion };