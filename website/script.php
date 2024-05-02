<?php
// Set the content type header to indicate that the response is an image
header('Content-Type: image/jpeg');

<<<<<<< HEAD
<!DOCTYPE html> 
<?php 
    setcookie("3allawi", "test", time() + 2 * 24 * 60 * 60); 
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
=======
// // Path to the image file
$imagePath = '/nfs/homes/ayylaaba/Desktop/last_update6/website/img/admin.jpeg';

// // Output the image file
readfile($imagePath);
echo "hello"
?>
>>>>>>> b52da9f07f97d5e13b410aa981028c4051d46904
