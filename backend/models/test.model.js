import mongoose from "mongoose";

const testSchema = new mongoose.Schema({
    name: {
        type: String,
        required: true,
    },
    description: {
        type: String,
    },
    students: [
        {
        type: mongoose.Schema.Types.ObjectId,
        ref: "User",
        },
    ],
    testDate: {
        type: String,
        required: true,
    },
    startTime: {
        type: String,
        required: true,
    },
    duration: {
        type: Number, 
        required: true,
    },
    questions: [
        {
            type: mongoose.Schema.Types.ObjectId,
            ref: "Question",
        },
    ],
    createdAt: {
        type: Date,
        default: Date.now,
    },
    updatedAt: {
        type: Date,
        default: Date.now,
    },
    createdBy: {
        type: mongoose.Schema.Types.ObjectId,
        ref: "User",
    },
});

const Test = mongoose.model("Test", testSchema);

export default Test;
