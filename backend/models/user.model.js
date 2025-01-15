import mongoose from "mongoose";

const userSchema = new mongoose.Schema({
    name: {
        type: String,
        required: true,
    },
    email: {
        type: String,
        required: true,
        unique: true,
    },
    role: {
        type: String,
        required: true,
        enum: ["student", "admin", "none"],
    },
    createdAt: {
        type: Date,
        default: Date.now,
    },
    questionsAttempted: [
        {
        question: {
            type: mongoose.Schema.Types.ObjectId,
            ref: "Question",
        },
        correct: {
            type: Boolean,
        },
        },
    ],
    pastTests: [
        {
        test: {
            type: mongoose.Schema.Types.ObjectId,
            ref: "Test",
        },
        score: {
            type: Number,
            required: true,
        },
        attemptedOn: {
            type: Date,
            required: true,
        },
        },
    ],

  //  for admins
  createdTests: [
    {
      test: {
        type: mongoose.Schema.Types.ObjectId,
        ref: "Test",
      },
      inProgress: {
        type: Boolean,
        default: true,
      },
    },
  ],
});

const User = mongoose.model("User", userSchema);

export default User;
