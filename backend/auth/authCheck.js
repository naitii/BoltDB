import axios from "axios";
const authCheck = async (req, res, next) => {
    const authHeader = req.headers["authorization"];
  const token = authHeader && authHeader.split(" ")[1]; // Extract Bearer token

  if (!token) {
    return res.status(401).json({ message: "Access token is missing" });
  }

  try {
    const response = await axios.get(`https://oauth2.googleapis.com/tokeninfo?access_token=${token}`);
    if (response.data && response.data.aud === process.env.GOOGLE_CLIENT_ID) {
      req.user = response.data; 
      return next();
    } else {
      return res.status(403).json({ message: "Invalid token" });
    }
  } catch (error) {
    return res.status(403).json({ message: "Token validation failed", error: error.response.data });
  }
}

export default authCheck;