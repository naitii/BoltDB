import express from 'express';
import * as questionController from '../controllers/question.controller.js';
import authCheck from '../auth/authCheck.js';

const router = express.Router();

router.post('/', authCheck, questionController.createQuestion);
router.put('/', authCheck, questionController.editQuestion);
router.delete('/', authCheck, questionController.deleteQuestion);

export default router;