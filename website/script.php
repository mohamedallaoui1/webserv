
<!DOCTYPE html> 
<?php 
    setcookie("user:", "mallaoui", time() + 2 * 24 * 60 * 60); 
?> 
<html> 
<body> 
    <?php 
        echo "cookie is created."
    ?> 
    <p> 
        <strong>Note:</strong>  
        You might have to reload the  
        page to see the value of the cookie. 
    </p> 
  
</body> 
</html>


<!-- <?php
// Set the content type header to indicate that the response is an image
// header('Content-Type: image/jpeg');

// // Path to the image file
// $imagePath = '/nfs/homes/ayylaaba/Desktop/last_update6/website/img/admin.jpeg';

// // Output the image file
// readfile($imagePath);
// echo "hello"
?> -->