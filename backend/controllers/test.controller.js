
const createTest = async (req, res) => {
    try{
        const user = req.user;
        if(!user){
            return res.status(401).json({message: "Unauthorized"});
        }
        if(user.role !== "admin"){
            return res.status(403).json({message: "Forbidden"});
        }
        const { name, description, duration, testDate } = req.body;
        if(!testName || !testDescription || !testDuration || !testInstructions || !testType){
            return res.status(400).json({message: "All fields are required"});
        }
        await Test.create({
            name,
            description,
            duration,
            testDate,

            createdBy: user._id
        });
        res.status(201).json({message: "Test Created Successfully"});
    }catch(err){
        res.status(500).json({message: "Internal Server Error"});
    }
};