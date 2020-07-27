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
    $config = json_decode(file_get_contents(dirname(__FILE__)."/config.json"));
    $mysqli=new mysqli($config->db_host, $config->db_name, $config->db_password, $config->db_table);

    $sql="SELECT id FROM customer WHERE 1 ";
    $stmt=$mysqli->prepare($sql);
    $stmt->execute();
    $stmt->bind_result($id);
    while($stmt->fetch()){
        $ids[]=$id;
    }
    $stmt->close();
    foreach($ids as $id){
        $date = time();

        $sql="SELECT a.level,b.sign,b.time,a.id  from customer as a left join log as b on a.id=b.id WHERE a.id=? order by b.time DESC limit 1";
        $stmt=$mysqli->prepare($sql);
        $stmt->bind_param('i',$id);
        $stmt->execute();
        $stmt->bind_result($level,$sign,$time,$id);
        $stmt->fetch();
        if($sign=='Alive')
        {
            $misstime=$date-strtotime($time);//string to int
            if($level=='LOW')
            {
               if($misstime>=30)
               {
                    alarm();
               }
            }
            else if($level=='MID')
            {
                if($misstime>=50)
                {
                    alarm();
                }
            }
            else if($level=='HIGH')
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