import dotenv from "dotenv";
import express from "express";
import connectDb from "./db/connectDb.js";
import cors from "cors";
import http from "http";
import authCheck from "./auth/authCheck.js";
import userRoute from "./routes/user.route.js";
import questionRoute from "./routes/question.route.js";
import testRoute from "./routes/test.route.js";
import passport from "passport";
import session from "express-session";
import GoogleStrategy from "passport-google-oauth20";
import User from "./models/user.model.js";

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

const session_secret = process.env.SESSION_SECRET;

app.use(
  session({
    secret: session_secret,
    resave: false,
    saveUninitialized: true,
    cookie: { secure: false },
  })
);

app.use(passport.initialize());
app.use(passport.session());

passport.use(new GoogleStrategy({
        clientID: process.env.GOOGLE_CLIENT_ID,
        clientSecret: process.env.GOOGLE_CLIENT_SECRET,
        callbackURL: "http://localhost:3000/auth/google/callback",
        scope: ["profile", "email"]
    },async (accessToken, refreshToken, profile, done) => {
        try {
            const user = await User.find({email: profile.emails[0].value})
            if(user.length === 0){
                const newUser = new User({
                  name: profile.displayName,
                  email: profile.emails[0].value,
                  role: "none",
                });
                await newUser.save();
            }
            return done(null, profile);
        } catch (error) {
            console.log(error);
        }
        return done(null, profile);
    }
));

passport.serializeUser((user, done) => {
    done(null, user);
})
passport.deserializeUser((user, done) => {
    done(null, user);
})


app.get("/auth/google", passport.authenticate("google", { scope: ["profile", "email"] }));
app.get("/auth/google/callback", passport.authenticate("google", { failureRedirect: "/" }), (req, res) => {     
    res.cookie("email", req.user.emails[0].value, {
      httpOnly: true,
      secure: true, // Use true in production with HTTPS
      sameSite: "none", // Adjust based on your environment
    });
    res.redirect("/protected");
});


app.use("/user", userRoute);
app.use("/question", questionRoute);
app.use("/test", testRoute);


app.get('/protected', authCheck, (req, res) => {
    res.send(`It is a protected route`);
});

app.get('/logout', (req, res) => {
  req.logout(function (err) {
    if (err) {
      return next(err);
    }
    res.redirect('/');
  });
});


server.listen(port, () => {
  console.log(`Server running on port ${port}`);
});