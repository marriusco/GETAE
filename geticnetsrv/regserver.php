<?php
	// User-Agent: GeticSrv
	// regserver.php?reg=%d&ip=%s&port=%d


	print_r($_GET);

//	if($_SERVER['HTTP_USER_AGENT']=="GeticSrv/1.0")
	{
		// read all file
		$servers = array();
		$index   = 0;
		$handle  = fopen("servers/servers.html","r");
		if($handle)
		{
			while (!feof($handle)) 
			{
				$line = fgets($handle, 1024);
				
				str_replace(" " ,"",$line);
				str_replace("\r" ,"",$line);
				str_replace("\n " ,"",$line);

				if(strlen($line) > 6)
				{
					$servers[$index++] = $line; 
				}
			}
			fclose($handle);
		}

		$record = $_GET['ip'].":".$_GET['port'];
			
		if($_GET['reg']==1)
		{
			$found = 0;
			foreach ($servers as $value) 
			{
				$len = strlen($value);
				if(substr($value,0,$len-2)== substr($record,0,$len-2))
				{
					$found=1;
					break;
				}
			}
			if($found==0)
			{
				$servers[$index++] = $record;
			}
		}
		else
		{
			while (list($key, $value) = each ($servers)) 
			{
				$len = strlen($value);
				if(substr($value,0,$len-2)== substr($record,0,$len-2))
				{
					$servers[$key]="*";
					break;
				}
			}
		}

		$handle  = fopen("servers/servers.html","w");
		foreach ($servers as $v) 
		{
			if($v != "*")
			{
				fwrite($handle, $v."\r\n");
			}
		}
		fclose($handle);
	}


?>

