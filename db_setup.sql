CREATE DATABASE IF NOT EXISTS university_system;
USE university_system;

CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    role ENUM('admin', 'user') DEFAULT 'user',
    collegeID VARCHAR(9) UNIQUE
);

CREATE TABLE IF NOT EXISTS tickets (
    id INT AUTO_INCREMENT PRIMARY KEY,
    firstName VARCHAR(50),
    middleName VARCHAR(50),
    lastName VARCHAR(50),
    collegeID VARCHAR(9),
    service VARCHAR(20),
    studentType VARCHAR(20),
    priority INT,
    importance INT,
    isVIP BOOLEAN DEFAULT FALSE,
    deadlineDays INT,
    description TEXT,
    arrivalTime DATETIME DEFAULT CURRENT_TIMESTAMP,
    status ENUM('pending', 'serving', 'served', 'cancelled') DEFAULT 'pending'
);

-- Insert a default admin
INSERT IGNORE INTO users (username, password, role) VALUES ('admin', 'admin123', 'admin');
