# nRF24-gateway

## 功能
- 接收 node 傳來的 data
- 傳至 server
- 判斷是否溺水


## 開機自動啟動
### setdies.php
- startgateway.sh
```shell
#!/bin/bash 
php /location/to/setdiesdata.php
```

- crontab
```
@reboot sleep 10; startgateway.sh
```

### gateway_Rf24
- /etc/rc.local
```
gateway_Rf24 2
```

---

## feature
- [ ] pipe
- [ ] Alarm



