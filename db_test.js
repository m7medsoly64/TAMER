const mysql = require('mysql2');
const db = mysql.createConnection({
    host: '127.0.0.1',
    user: 'root',
    password: '',
    database: 'university_system'
});

console.log('Attempting to connect to MySQL...');
db.connect((err) => {
    if (err) {
        console.log('-----------------------------------------');
        console.log('FAILED TO CONNECT!');
        console.log('Error Code: ' + err.code);
        console.log('Error Message: ' + err.message);
        console.log('-----------------------------------------');
        process.exit(1);
    } else {
        console.log('-----------------------------------------');
        console.log('SUCCESS! Connection established.');
        console.log('-----------------------------------------');
        db.end();
        process.exit(0);
    }
});
