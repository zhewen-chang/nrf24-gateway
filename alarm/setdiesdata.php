<?php

while(1) {
    isdie();
}

function alarm()
{
    system("gpio -g mode 21 out");
    for ($i=0; $i<3; $i++) {
        system("gpio -g write 21 1");
        usleep(500000);
        system("gpio -g write 21 0");
        usleep(500000);
    }
}

function isdie()
{
    $config = json_decode(file_get_contents("/home/pi/RF24_gateway/alarm/config.json"));
    $mysqli=new mysqli($config->db_host, $config->db_name, $config->db_password, $config->db_table);

    $sql="SELECT id FROM customer WHERE 1 ";
    $stmt=$mysqli->prepare($sql);
    $stmt->execute();
    $stmt->bind_result($id);
    while($stmt->fetch()){
        $ids[]=$id;
    }
    foreach($ids as $id){
        $date = time();

        $sql="SELECT `id`, `level`, `time`, `sign` FROM log WHERE id =? order by time desc";
        $stmt=$mysqli->prepare($sql);
        $stmt->bind_param('i',$id);
        $stmt->execute();
        $stmt->bind_result($id,$level,$time,$sign);
        $stmt->fetch();
        if($sign=='Alive')
        {
            $misstime=$date-strtotime($time);//string to int
            if($level=='Low')
            {
               if($misstime>=30)
               {
                    alarm();
               }
            }
            else if($level=='Mid')
            {
                if($misstime>=50)
                {
                    alarm();
                }
            }
            else if($level=='High')
            {
                if($misstime>=70)
                {
                    alarm();
                }
            }
            else
            {
                echo "bad code or bad data passing.";
            }
                       
        }
        $stmt->close();
    }    

    $mysqli->close();
}
?>