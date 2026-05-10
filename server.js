const express = require('express');
const mysql = require('mysql2');
const bodyParser = require('body-parser');
const path = require('path');
const cors = require('cors');

const app = express();
const port = 3000;

// Middleware
app.use(cors());
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, 'public')));

// MySQL Connection (Update with your credentials)
const db = mysql.createConnection({
    host: 'localhost',
    user: 'root',
    password: '', // Enter your MySQL password
    database: 'university_system'
});

db.connect((err) => {
    if (err) {
        console.error('Error connecting to MySQL:', err);
        return;
    }
    console.log('Connected to MySQL database');
});

// Priority Logic Helpers (Translated from C++)
const getServicePriority = (service) => {
    const priorities = { 'emergency': 5, 'government': 4, 'academic': 3, 'financial': 2 };
    return priorities[service] || 1;
};

const getBaseImportance = (service) => {
    const importance = { 'emergency': 5, 'academic': 4, 'government': 3, 'financial': 2 };
    return importance[service] || 1;
};

// --- API Routes ---

// Login
app.post('/api/login', (req, res) => {
    const { username, password } = req.body;
    const query = 'SELECT * FROM users WHERE username = ? AND password = ?';
    db.query(query, [username, password], (err, results) => {
        if (err) return res.status(500).json({ error: err.message });
        if (results.length > 0) {
            res.json({ success: true, user: results[0] });
        } else {
            res.status(401).json({ success: false, message: 'Invalid credentials' });
        }
    });
});

// Register
app.post('/api/register', (req, res) => {
    const { username, password, collegeID } = req.body;
    const query = 'INSERT INTO users (username, password, collegeID, role) VALUES (?, ?, ?, "user")';
    db.query(query, [username, password, collegeID], (err, result) => {
        if (err) {
            if (err.code === 'ER_DUP_ENTRY') return res.status(400).json({ success: false, message: 'Username or College ID already exists' });
            return res.status(500).json({ error: err.message });
        }
        res.json({ success: true, message: 'Registration successful' });
    });
});

// Book Ticket
app.post('/api/tickets', (req, res) => {
    const t = req.body;
    
    // Calculate Priority and Importance
    const typePriority = getServicePriority(t.service);
    let importance = getBaseImportance(t.service);
    
    if (t.studentType === 'graduate') importance += 2;
    else if (t.studentType === 'final') importance += 1;
    
    if (t.deadlineDays <= 1) importance += 2;
    else if (t.deadlineDays <= 3) importance += 1;

    const query = `INSERT INTO tickets 
        (firstName, middleName, lastName, collegeID, description, service, studentType, priority, importance, isVIP, deadlineDays) 
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`;
    
    const values = [
        t.firstName, t.middleName, t.lastName, t.collegeID, t.description,
        t.service, t.studentType, typePriority, importance, 
        t.isVIP, t.deadlineDays
    ];

    db.query(query, values, (err, result) => {
        if (err) return res.status(500).json({ error: err.message });
        res.json({ success: true, ticketId: result.insertId });
    });
});

// Get Tickets (Sorted by Priority for Admin)
app.get('/api/tickets/queue', (req, res) => {
    // Standard SQL ordering to mimic Priority Queue
    // 1. typePriority (DESC)
    // 2. importance (DESC)
    // 3. isVIP (DESC)
    // 4. arrivalTime (ASC)
    const query = `
        SELECT *, 
        (importance + (TIMESTAMPDIFF(MINUTE, arrivalTime, NOW()) / 5)) as current_importance
        FROM tickets 
        WHERE status = 'pending' 
        ORDER BY priority DESC, current_importance DESC, isVIP DESC, arrivalTime ASC`;
    
    db.query(query, (err, results) => {
        if (err) return res.status(500).json({ error: err.message });
        res.json(results);
    });
});

// Serve Ticket
app.put('/api/tickets/serve/:id', (req, res) => {
    const id = req.params.id;
    const query = "UPDATE tickets SET status = 'served' WHERE id = ?";
    db.query(query, [id], (err, result) => {
        if (err) return res.status(500).json({ error: err.message });
        res.json({ success: true });
    });
});

// Cancel Ticket
app.put('/api/tickets/cancel', (req, res) => {
    const { collegeID } = req.body;
    const query = "UPDATE tickets SET status = 'cancelled' WHERE collegeID = ? AND status = 'pending'";
    db.query(query, [collegeID], (err, result) => {
        if (err) return res.status(500).json({ error: err.message });
        res.json({ success: true, affectedRows: result.affectedRows });
    });
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});
