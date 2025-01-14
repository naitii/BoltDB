import jwt from "jsonwebtoken";

function authCheck(req, res, next) {
    const authHeader = req.headers.authorization;
    // const token = authHeader && authHeader.split(" ")[1];
    const token = req.token;
    if(!token) {
        return res.status(401).json({message: "Unauthorized"});
    }
    jwt.verify(token, process.env.ACCESS_TOKEN, (err, email) => {
        if(err) {
            return res.status(403).json({message: "Forbidden"});
        }
        next();
    });

}

export default authCheck;