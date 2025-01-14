
const createQuestion = async (req, res) => {
    console.log(req.user)
    try {
        const user = req.user;
        if (!user) {
            return res.status(401).json({ message: "Unauthorized" });
        }
        if (user.role !== "admin") {
            return res.status(403).json({ message: "Forbidden" });
        }
        const {
          questionText,
          questionType,
          questionImage,
          subject,
          topic,
          positiveMarks,
          negativeMarks,
          testId,
        } = req.body;

        if (!questionText || !questionType) {
            return res.status(400).json({ message: "Question Text and it's type is required!" });
        }

        if(questionType === "Multiple Choice" || questionType === "Multiple Correct") {
            const { options, correctOptions } = req.body;
            if (!options || !correctOptions) {
                return res.status(400).json({ message: "Options and Correct Options are Required" });
            }

            await Question.create({
              questionText,
              questionType,
              questionImage,
              subject,
              topic,
              positiveMarks,
              negativeMarks,
              options,
              correctOptions,
              testId,
            });
        }

        if(questionType === "Integer Type") {
            const { integerAnswer } = req.body;
            if (!integerAnswer || !Number.isInteger(integerAnswer)) {
                return res.status(400).json({ message: "Integer Answer is Required" });
            }

            await Question.create({
              questionText,
              questionType,
              questionImage,
              subject,
              topic,
              positiveMarks,
              negativeMarks,
              integerAnswer,
              testId,
            });
        }

        if(questionType === "Numerical Type") {
            const { numericalAnswer } = req.body;
            if (!numericalAnswer || !numericalAnswer.correctAnswer || !numericalAnswer.tolerance) {
                return res.status(400).json({ message: "Numerical Answer and Tolerance is Required" });
            }
            await Question.create({
              questionText,
              questionType,
              questionImage,
              subject,
              topic,
              positiveMarks,
              negativeMarks,
              numericalAnswer,
              testId,
            });
        }

    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};




export { createQuestion };