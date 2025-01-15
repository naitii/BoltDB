import jwt from "jsonwebtoken";

function authCheck(req, res, next) {
    const token =
      req.headers.cookie.substring(6) ||
      req.headers.authorization?.split(" ")[1];
    
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