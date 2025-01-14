import mongoose from "mongoose";

const questionSchema = new mongoose.Schema({
  questionText: {
    type: String,
    required: true,
  },
  questionImage: {
    type: String,
  },
  testId: {
    type: String,
    required: true,
  },
  questionType: {
    type: String,
    required: true,
    enum: [
      "Multiple Choice",
      "Multiple Correct",
      "Integer Type",
      "Numerical Type",
    ],
  },
  options: [
    {
      text: {
        type: String,
      },
      image: {
        type: String,
      },
    },
  ],
  correctOptions: [
    {
      type: String,
    },
  ],
  integerAnswer: {
    type: Number,
  },
  numericalAnswer: {
    correctAnswer: {
      type: Number,
    },
    tolerance: {
      type: Number,
    },
  },
  subject: {
    type: String,
    required: true,
    enum: ["Physics", "Chemistry", "Mathematics", "Biology"],
  },
  topic: {
    type: String,
    required: true,
  },
  positiveMarks: {
    type: Number,
    required: true,
    default: 4,
  },
  negativeMarks: {
    type: Number,
    required: true,
    default: -1,
  },
  createdAt: {
    type: Date,
    default: Date.now,
  },
});

const Question = mongoose.model("Question", questionSchema);

export default Question;
