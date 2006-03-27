<?
class LogEntry {
  var $author;
  var $files;
  var $msgs;

  function LogEntry() {
    $this->author = '';
    $this->files = array();
    $this->msgs = array();
  }
}

class NewsLog {
  
  var $entries;

  function NewsLog() {    
    $vermap = $this->getVersions();

    $oldr = null;
    foreach($vermap as $tag => $rev) {
      $xml = simplexml_load_string($this->getLog($oldr, $rev));
      $oldr = $rev;

      foreach($xml->logentry as $log) {
        $this->processEntry($tag, $log);
      }
    }    
  }

  function getVersions() {
    $output = `svn ls --verbose https://svn.sourceforge.net/svnroot/protoeditor/tags`;
    
    $tags = array();
    $lines = explode("\n", $output);
    foreach($lines as $line) {
      if(strlen(trim($line)) == 0) continue;
      $tokens = explode(' ', $line);
      $tkversion = array_slice($tokens, -1);      
      array_push($tags, substr($tkversion[0], 0, strlen($tkversion[0])-1));
    }

      
    $versions = array();  
    foreach($tags as $tag) {
      $output = `svn log -v --stop-on-copy https://svn.sourceforge.net/svnroot/protoeditor/tags/$tag`;
      $lines = explode("\n", $output);
      $words = explode(' ', $lines[1]);      
      $versions[$tag] = substr($words[0], 1); //removes the prefix 'r'
    }

    return $versions;
  }

  function getLog($oldr, $rev) {
    if(!$oldr) {
      $filec = `svn log --xml -v -r $rev https://svn.sourceforge.net/svnroot/protoeditor/trunk`;
    } else {
      $filec = `svn log --xml -v -r $oldr:$rev https://svn.sourceforge.net/svnroot/protoeditor/trunk`;
    }

    return $filec;
  }

  function processEntry($tag, $log) {
    $date = $this->processDate((string)$log->date);
    $entry = new LogEntry();
    $entry->author = (string)$log->author;

    $msg = $this->processMsg((string)$log->msg);
    if($msg) {
      $entry->msgs = $msg;
    }

    $entry->files = $this->processFiles($log->paths);

    if(!isset($this->entries[$tag])) {
      $this->entries[$tag] = array();
    }

    array_push($this->entries[$tag], $entry);
  }

  function getEntry($date) {
    foreach($this->entries as $entry) {
      if($this->processDate($date) == $entry->date) {
        return $entry;
      }
    }
    return null;
  }

  function processDate($date) {
    //"2006-03-19T01:04:36.108693Z"
    return substr($date, 0, 10);
  }

  function processMsg($logmsg) {
    //1: ignores "nomsg" and "*** empty log message ***"
    //2: break into BUGFIX, NEWS, REGULAR
    //4: remove initial "-"

    $lns = explode("\n", $logmsg);
    $lines = array();
    foreach($lns as $l) {
      array_push($lines, trim($l));
    }

    $msgs["NEW"] = array();
    $msgs["BUGFIX"] = array();
    $msgs["DEVNULL"] = array();

    $flag = "DEVNULL";
    foreach($lines as $line) {
      
      if(!$line) continue;

      if((strpos($line, "nomsg")!==FALSE) || (strpos($line, "*** empty log message ***")!==FALSE)) {
        continue;
      }

      if((strpos($line, "NEW")!==FALSE) || (strpos($line, "FEATURE")!==FALSE)) {
        $flag = "NEW";  
        continue;
      } else if(strpos($line, "FIX")!==FALSE) {
        $flag = "BUGFIX";
        continue;
      }   

      if(substr($line, 0, 1) == '-') {
        $line = substr($line, 1);
      }
      array_push($msgs[$flag], $line);
    }

    if((count($msgs["NEW"]) == 0) &&
       (count($msgs["BUGFIX"]) == 0) ) {
      return null;
    } else {
      return $msgs;
    }
  }

  function processFiles($files) {
    if(!count($files->path)) {
      return null;
    }
    
    $ret = array();
    foreach($files->path as $file) {
      array_push($ret, $file);
    }
    return $ret;
  }

  function dumpNewsLog() {    

    $this->entries = array_reverse($this->entries, true);
    foreach($this->entries as $version => $logs) {
      $printedMsgs = array();

      if(!$this->hasNewsMessages($logs)) {
        //don't print this day
        continue;
      }

      $version = strtolower($version);

      if(substr($version, 0, 4) == "beta") {
        $version = substr($version, 5);
      }
      
      echo "Version ".str_replace('_', '.', $version);
      
      foreach($logs as $log) {
        if(!$log->msgs) {
          continue;
        }

        if(!count($log->files)) {
          continue;
        }
        
        if((count($log->msgs['NEW']) == 0) &&
           (count($log->msgs['BUGFIX']) == 0) ) {
          continue;
        }

        echo "\n\n";
        foreach($log->files as $file) {
          echo "\t* $file:\n";
        }

        if(count($log->msgs['NEW'])) {          
          foreach($log->msgs['NEW'] as $new) {
            $this->printMsg($new, "\n\t-");
          }
        }

        if(count($log->msgs['BUGFIX'])) {
          echo "\n\tBug fixes:";
          foreach($log->msgs['BUGFIX'] as $new) {            
            $this->printMsg($new, "\n\t-");
          }
        }
      }
      echo "\n\n";      
    }
  }

  function hasNewsMessages($logs) {
    //check only for news and bugfixes
    foreach($logs as $log) {
      if(count($log->msgs)==0) continue;

      if((count($log->msgs['NEW']) > 0) ||
         (count($log->msgs['BUGFIX']) > 0)) {
        return true;
      }
    }
    return false;
  }

  function printMsg($msg, $indent) {
    //messages > 60 should be exploded in multiple lines
    if(strlen($msg) < 60) {
      echo $indent.$msg;
    } else {
      $lines  = array();
      $words = explode(' ', $msg);
      $num = count($words);
      while(1) {
        $line = implode(' ', array_slice($words, 0, $num));
        if(strlen($line) > 60) {
          $num--;
        } else {
          array_push($lines, $line);
          $words = array_slice($words, $num);
          $num = count($words);
          if(!count($words)) break;
        }
      }
      
      $first = $indent;
      $other = substr($indent, 0, strlen($indent)-1);
      $id = $first;
      foreach($lines as $line) {
        echo $id.$line;
        $id = $other;
      }
    } 
  }
}


  $cl = new NewsLog();
  $cl->dumpNewsLog();
?>