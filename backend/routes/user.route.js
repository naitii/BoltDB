import express from "express";
import * as userController from "../controllers/user.controller.js";
import authCheck from "../auth/authCheck.js";

const router = express.Router();

router.get("/", authCheck, userController.getUser);
router.put("/role", authCheck, userController.setRole);
// router.post("/signup", userController.signup);
// router.post("/login", userController.login);


export default router;