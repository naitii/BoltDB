import dotenv from "dotenv";
import express from "express";
import connectDb from "./db/connectDb.js";
import cors from "cors";
import http from "http";
import authCheck from "./auth/authCheck.js";
import userRoute from "./routes/user.route.js";

dotenv.config();


const app = express();

app.use(
  cors({
    origin: "http://localhost:5173",
    methods: ["GET", "POST", "PUT", "DELETE"],
    allowedHeaders: ["Content-Type", "Authorization"],
    credentials: true,
  })
);

const server = http.createServer(app);

const port = process.env.PORT || 5000;

connectDb();

app.use(express.json());
app.use("/user", userRoute);


app.get('/protected', authCheck, (req, res) => {
    res.send('Protected route');
});

app.get('/logout', (req, res) => {
  req.logout(function (err) {
    if (err) {
      return next(err);
    }
    res.redirect("/");
  });
});




server.listen(port, () => {
  console.log(`Server running on port ${port}`);
});