<?php
    $config = json_decode(file_get_contents("../alarm/config.json"));
    $mysqli = new mysqli($config->db_host, $config->db_name, $config->db_password, $config->db_table);
    $sql="SELECT id FROM customer order by id desc";
    $stmt=$mysqli->prepare($sql);
    $stmt->execute();
    $stmt->bind_result($id);  
    $max=0;
    while($stmt->fetch()){
        if($id>$max)
            $max=$id;
    }
    $max=$max+1;
    if($max<10)
        $max="00".$max;
    else if(max<100)
        $max="0".$max;
    $stmt->close();
    $mysqli->close();
    $file = fopen("id.txt","w");
    fwrite($file,$max);
    fclose($file);
?>