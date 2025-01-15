import express from "express";
import * as testController from "../controllers/test.controller.js";
import authCheck from "../auth/authCheck.js";

const router = express.Router();

router.post("/", authCheck, testController.createTest);
router.delete("/", authCheck, testController.deleteTest);
router.get("/", authCheck, testController.getTests);

export default router;
