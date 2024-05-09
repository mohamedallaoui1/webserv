
<?php

error_reporting(E_ALL);
ini_set('display_errors', 1);

// Check if form was submitted (optional, but good practice)
if ($_SERVER['REQUEST_METHOD'] === 'POST') {

  $username = $_POST['username'];
  $password = $_POST['password'];

  echo "<html><body>";
  echo "username: $username<br>";
  echo "password: $password<br>";  // Consider using password hashing for security
  echo "</body></html>";

} else {
  // Handle the case where the form wasn't submitted (optional, but informative)
  echo "Please submit the form.";
}
?>