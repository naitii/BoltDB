import express from 'express';
import * as questionController from '../controllers/question.controller.js';
import authCheck from '../auth/authCheck.js';

const router = express.Router();

router.post('/questions', authCheck, questionController.createQuestion);

export default router;