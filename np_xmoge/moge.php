<html>

<table border="1" width="100%">
	<tr>
	<td valign='top'>
	    <form name='xxx'><br>
        Proxy  :<input type="text" name="SRV" size="20"> 
        Proxy  Port:  <input type="text" name="PRT" size="16">
        <SELECT SIZE="1" NAME="mnu" onchange="clickIt(this)">
            <?php

                if ($handle = opendir('levels200')) 
                {
                    while (false !== ($file = readdir($handle))) 
                    { 
                        if(strstr($file,'.ml') && !strstr($file,'index'))
                            echo "<option>{$file}</option>";
                    }
                }
                closedir($handle); 
            ?>
    </select>
    </form>
	</td>
	</tr>
	<tr>
	<td ><div align='center'>
		    <embed id="xpmoge" type="application/xmoge-gbt" width=640 height=480>
		        <OBJECT ID="xmoge" width="640" height="480" 
                CLASSID="CLSID:5D32D59F-942D-40AB-8D8C-F78EA015B2E7" CODEBASE="http://www.zalsoft.com/xmoge.dll">
                <PARAM NAME="LoadLevel" VALUE="http://linux/levels200/default.ml" />
                [Object not available! Download and register the dll: regsvr32 xmoge.dll?]
                </OBJECT>
            </embed>
        </div>
     </td>
    </tr>
</table>
</body>


<SCRIPT>
function clickIt(select)  
{
    var lv   = "http://linux/levels200/";
    var seli = select.selectedIndex;
    var st   = select.options[seli].text;
    
    if(document.xxx.SRV.value && document.xxx.PRT.value)
        xpmoge.SetProxy(document.xxx.SRV.value, document.xxx.PRT.value);
    if(st)
    {
        lv += st;
        var xpmoge = document.getElementById('xpmoge');
        xpmoge.LoadLevel(lv);
    }    
}


</SCRIPT>

</html>


