
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
