<?php
require("login/login.php");
include 'monitor.inc';
include_once 'graph_functions.inc';
include_once 'firephp/0.3.2/fb.php';

$userId = getCurrentUserId();
date_default_timezone_set($_SESSION['ls_timezone']);

// Set action
if (isset($_REQUEST['act'])){
  $action = $_REQUEST['act'];
} else {
  $action = 'default';
}
$smarty->assign('action',$action );
// Default chart time
$smarty->assign('chartType', 'Line');

if (isset($_GET['aggregateMethod'])) {
	$_SESSION['aggregateMethod'] = $_GET['aggregateMethod'];
} elseif(!isset($_SESSION['aggregateMethod'])) {
	$_SESSION['aggregateMethod'] = "avg";
}

$smarty->assign('aggregateMethod', $_SESSION['aggregateMethod']);
// Show only active jobs
// Show inactive jobs
  if ( isset($_REQUEST['showInactiveJobsGraph']) ) {
    if ( $_REQUEST['showInactiveJobsGraph'] == 1 || $_REQUEST['showInactiveJobsGraph'] == "true"){
      $_SESSION['showInactiveJobsGraph'] = true;
    } else {
      $_SESSION['showInactiveJobsGraph'] = false;
    }
   } else if ( !isset($_SESSION['showInactiveJobsGraph'] ) ){
    $_SESSION['showInactiveJobsGraph'] = false;
  }
  $showInactiveJobs = $_SESSION['showInactiveJobsGraph'];
  $smarty->assign('showInactiveJobsGraph', $showInactiveJobs);
// Folder handling
if (isset($_REQUEST['folderId'])) {
  $_SESSION['jobsFolderId'] = $_REQUEST['folderId'];
}

if (!isset($_SESSION['jobsFolderId'])) {
  $_SESSION['jobsFolderId'] = getRootFolderForUser($userId,'WPTJob');
}
$folderId = $_SESSION['jobsFolderId'];

$smarty->assign('folderId', $_SESSION['jobsFolderId']);

$folderTree = getFolderTree($userId, 'WPTJob');
$smarty->assign('folderTree', $folderTree);
// End Folder handling


if (!isset($_REQUEST['includeRepeatView'])) {
  $includeRepeatView = 0;
} else {
  $includeRepeatView = 1;
}
$smarty->assign('includeRepeatView', $includeRepeatView);

// Start/end times
// timeFrame > 0 will ignore time select boxes
$timeFrame = '';
$smarty->assign('timeFrame',$timeFrame);
if (isset($_REQUEST['timeFrame']) && (($timeFrame = $_REQUEST['timeFrame']) > 0)) {
  $smarty->assign('timeFrame',$timeFrame);
  $endDateTime = gmdate('U') + 3600;
  $startDateTime = $endDateTime - $timeFrame;
} else {
  if (isset($_REQUEST['startMonth'])) {
    $startDateTime = mktime($_REQUEST['startHour'], 0, 0, $_REQUEST['startMonth'], $_REQUEST['startDay'], $_REQUEST['startYear']);
  }
  if (isset($_REQUEST['endMonth'])) {
    $endDateTime = mktime($_REQUEST['endHour'], 59, 59, $_REQUEST['endMonth'], $_REQUEST['endDay'], $_REQUEST['endYear']);
  }
}
// no end date, use now.
if (!isset($endDateTime) || $endDateTime > time()) {
  $endDateTime = gmdate('U') + 3600;
}
// no start date, use last 7 days.
if (!isset($startDateTime)) {
  $startDateTime = $endDateTime - 604800;
}

$smarty->assign('startTime', $startDateTime);
$smarty->assign('endTime', $endDateTime);
// End start/end times

if(!isset($_REQUEST['todStartHour'])) {
    $todStartHour = 0;
} else {
    $todStartHour = (int)$_REQUEST['todStartHour'];
}

if(!isset($_REQUEST['todEndHour'])) {
    $todEndHour = 0;
} else {
    $todEndHour = (int)$_REQUEST['todEndHour'];
}


$smarty->assign('todStartHourTimestamp', mktime($todStartHour));
$smarty->assign('todEndHourTimestamp', mktime($todEndHour));

// Create jobs list
$q = Doctrine_Query::create()->select('j.Id, j.Label')
        ->from('WPTJob j')
        ->orderBy('j.Label')
        ->setHydrationMode(Doctrine_Core::HYDRATE_ARRAY);
    if ($folderId > -1 && hasPermission('WPTJob',$folderId,PERMISSION_READ)) {
      $q->andWhere('j.WPTJobFolderId = ?', $folderId);
    } else {
      $q->andWhere('j.UserId = ?', $user_id);
    }
    if (!$showInactiveJobs){
      $q->andWhere('j.Active = ?',true);
    }

//if ($folderId > -1) {
//  $q->andWhere('j.WPTJobFolderId = ?', $folderId);
//}
$shares = getFolderShares($userId,'WPTJob');
$smarty->assign('shares',$shares);

$jobs = $q->fetchArray();
$q->free(true);
$jobArray = array();

foreach ($jobs as $j) {
  $i = $j['Id'];
  $l = $j['Label'];
  $jobArray[$i] = $l;
}
$smarty->assign('jobs', $jobArray);
// End create jobs list

// Start field handling
// Add availabe field keys
//$availFieldKeysFV = array('FV_TTFB' => 'FV_TTFB', 'FV_Render' => 'FV_Render', 'FV_Doc' => 'FV_Doc', 'FV_Dom' => 'FV_Dom', 'FV_Fully' => 'FV_Fully');
$availFieldKeysFV = array('FV_TTFB', 'FV_Render', 'FV_Doc', 'FV_Dom', 'FV_Fully', 'FV_SI');

//$availFieldKeysRV = array('RV_TTFB' => 'RV_TTFB', 'RV_Render' => 'RV_Render', 'RV_Doc' => 'RV_Doc', 'RV_Dom' => 'RV_Dom', 'RV_Fully' => 'RV_Fully');
$availFieldKeysRV = array('RV_TTFB', 'RV_Render', 'RV_Doc', 'RV_Dom', 'RV_Fully', 'RV_SI');
$smarty->assign('availFieldKeysFV', $availFieldKeysFV);
$smarty->assign('availFieldKeysRV', $availFieldKeysRV);

$histogramResolutionOptions = array(1 => '1ms', 2 => '2ms', 5 => '5ms', 10 => '10ms', 25 => '25ms', 50 => '50ms', 100 => '100ms', 250 => '250ms', 500 => '0.5s', 1000 => '1s');
$smarty->assign('histogramResolutionOptions', $histogramResolutionOptions);
// Process fields to display
if ( isset($_REQUEST['fields'])){
  $fieldsToDisplay = $_REQUEST['fields'];
}
$availFields = array();
$availFields['FV_TTFB'] = "AvgFirstViewFirstByte";
$availFields['FV_Render'] = "AvgFirstViewStartRender";
$availFields['FV_Doc'] = "AvgFirstViewDocCompleteTime";
$availFields['FV_Dom'] = "AvgFirstViewDomTime";
$availFields['FV_Fully'] = "AvgFirstViewFullyLoadedTime";
$availFields['RV_TTFB'] = "AvgRepeatViewFirstByte";
$availFields['RV_Render'] = "AvgRepeatViewStartRender";
$availFields['RV_Doc'] = "AvgRepeatViewDocCompleteTime";
$availFields['RV_Dom'] = "AvgRepeatViewDomTime";
$availFields['RV_Fully'] = "AvgRepeatViewFullyLoadedTime";

if (isset($fieldsToDisplay) && sizeof($fieldsToDisplay) > 0) {
  $fields = array();
  foreach ($fieldsToDisplay as $field) {
    if (!empty($field))
      $fields[$field] = $availFields[$field];
  }
} else {
  //  $fields = $availFields;
  $fields["FV_Fully"] = $availFields['FV_Fully']; // default selection
  foreach ($fields as $key => $field) {
    $fieldsToDisplay[] = $key;
  }
}

$smarty->assign('fieldsToDisplay', $fieldsToDisplay);
// End field handling
if ( isset($_REQUEST['job_id']) ){
  $jobIds = $_REQUEST['job_id'];
} else{
  $jobIds = '';
}
$smarty->assign('job_ids', $jobIds);

if (isset($_REQUEST['adjustUsing'])){
  $adjustUsing = $_REQUEST['adjustUsing'];
}
if (empty($adjustUsing)) {
  $adjustUsing = "AvgFirstViewDocCompleteTime";
}
$smarty->assign('adjustUsing', $adjustUsing);

if (!isset($_REQUEST['percentile'])) {
  $percentile = 1;
} else {
  $percentile = $_REQUEST['percentile'];
}
$smarty->assign('percentile', $percentile);
if ( isset($_REQUEST['trimAbove'])){
  $trimAbove = $_REQUEST['trimAbove'];
}else {
  $trimAbove ="";
}
$smarty->assign('trimAbove', $trimAbove);
if (isset($_REQUEST['trimBelow'])){
  $trimBelow = $_REQUEST['trimBelow'];
}else{
  $trimBelow="";
}
$smarty->assign('trimBelow', $trimBelow);
if ( isset($_REQUEST['interval'])){
  $interval = $_REQUEST['interval'];
} else {
  $interval = 'Auto';
}
$smarty->assign('interval', $interval);

if (!isset($_REQUEST['act'])) {
  $smarty->display('report/flashGraph.tpl');
  exit;
}

if ($_REQUEST['act'] == 'report') {
  $series = getSeriesDataForMultiJobs($userId,$jobIds,$startDateTime,$endDateTime,$interval,false);
  $changeNotes = getChangeNoteData( $userId, $startDateTime, $endDateTime );
  // Add change notes
  $smarty->assign('changeNotes',$changeNotes);

  $timeStamps = array();
//  foreach($series as $ser){
//    $timeStamps[]=$ser['Date'];
//  }
//  array_walk($timeStamps, 'formatDate');
  $jobTable = Doctrine_Core::getTable('WPTJob');
  $datas = array();
  $overallAverages = array();
  $averageDetails = array();
  foreach($jobIds as $jobId){
    $responseTimes = getGraphData( $jobId, $startDateTime, $endDateTime, $percentile, $trimAbove, $adjustUsing, $trimBelow, $todStartHour, $todEndHour );

    $avg = getResultsDataAvg($startDateTime, $endDateTime, $interval, $responseTimes, $fields, $_SESSION['aggregateMethod']);

    foreach($availFields as $availField){
      $data = array();
      $idx = 0;
      foreach($avg as $a){
        $date = $a['Date'];
        if (array_key_exists($availField,$a)){
          if ( empty($a[$availField])){
            continue;
          }
          $data[$date] = $a[$availField]/1000;
          $timeStamps[$idx] = date('m/d H:i', $date);
          $idx++;
        }
      }
//      $timeStamps[$idx] = date('m/d H:i', $endDateTime);
      if ( sizeof($data) > 0){
        $job = $jobTable->find($jobId);
        $jobLabel = $job['Label'];
        $datas[$jobLabel." - ".$availField] = $data;
      }
    }
//    print_r($timeStamps);
//    echo sizeof($datas['www.wine.com Treated East IE7 - AvgFirstViewDocCompleteTime']);exit;
//    print_r($datas);exit;
    $allFields = $availFields;
    $allFields[] = 'AvgFirstViewDocCompleteRequests';
    $allFields[] = 'AvgFirstViewDocCompleteBytesIn';
    $allFields[] = 'AvgRepeatViewFullyLoadedRequests';
    $allFields[] = 'AvgRepeatViewFullyLoadedBytesIn';
    $allFields[] = 'AvgRepeatViewDocCompleteRequests';
    $allFields[] = 'AvgRepeatViewDocCompleteBytesIn';
    $allFields[] = 'AvgFirstViewFullyLoadedRequests';
    $allFields[] = 'AvgFirstViewFullyLoadedBytesIn';

    $overallAverage = getResultsDataAvg($startDateTime, $endDateTime, $endDateTime - $startDateTime, $responseTimes, $allFields, $_SESSION['aggregateMethod']);
    $overallAverage['Label']=$jobLabel;
    $overallAverages[] = $overallAverage;

    $avgDetail = getResultsDataAvg($startDateTime, $endDateTime, $interval, $responseTimes, $allFields, $_SESSION['aggregateMethod']);
    $avgDetail['Label']=$jobLabel;
    $averageDetails[] = $avgDetail;
  }
  $smarty->assign('interval',$interval);
  $smarty->assign('overallAverages',$overallAverages);
  $smarty->assign('averageDetails',$averageDetails);
  $cryptQueryString = compressCrypt(urldecode($_SERVER['QUERY_STRING'].'&_pky='.$userId));
  $smarty->assign('cryptQueryString',$cryptQueryString);
  $smarty->assign('datas',$datas);
  $smarty->assign('x_axis_tick_labels',$timeStamps);

  // If the user is a guest then only show the shared report format
  if (isset($_SESSION['ls_guest'])){
    $smarty->display('report/share.tpl');
    session_unset();
    exit;
  }
  $smarty->display('report/flashGraph.tpl');
  exit;
}
if ($_REQUEST['act'] == 'download') {
  $downloadData = array();
  $jobTable= Doctrine_Core::getTable('WPTJob');
  $flds[]='Job';
  $flds += $fields;
  foreach ($jobIds as $jobId) {
    $job= $jobTable->find($jobId);
    $jobName = $job['Label'];
    $datas = getGraphData( $jobId, $startDateTime, $endDateTime, $percentile, $trimAbove, $adjustUsing, $trimBelow, $todStartHour, $todEndHour );

    if ($interval > 1) {
      $datas = getResultsDataAvg($startDateTime, $endDateTime, $interval, $datas, $flds, $_SESSION['aggregateMethod']);
    }

    foreach ($datas as $key => $data) {
      $data['Date'] = date('Y/m/d,H:i:s', $data['Date']);
      $data['Job'] = $jobName;
      $downloadData[] = $data;
    }
  }

  $header = 'Date,Time,Job,';
  $last_item = end($fieldsToDisplay);

  foreach ($fieldsToDisplay as $f) {
    $header .= $f;
    if ($f != $last_item) {
      $header .= ",";
    }
  }
  ob_get_clean();
  header("Content-Type: text/csv");
  header("Content-Disposition: attachment; filename=\"data-".date('YMd_His',$startDateTime).'-'.date('YMd_His',$endDateTime).".csv\"");
  echo $header . "\n";
  outputCSV($downloadData);
  exit;
}
// ************** Chart Processing **************
if ($_REQUEST['act'] == 'graph') {
  $smarty->assign('wptResultURL', $wptResult);
  $smarty->display('report/flashGraph.tpl');
}

function formatDate(&$aVal) {
  $aVal = date('m/d H:i', $aVal);
}

function outputCSV($data) {
  $outstream = fopen("php://output", 'w');

  function __outputCSV(&$vals, $key, $filehandler) {
    fputcsv($filehandler, $vals, ',', ' ');
  }

  array_walk($data, '__outputCSV', $outstream);

  fclose($outstream);
}
function cleanupDir($dir, $timeStamp){
  $files = array();
  $index = array();
  $time = gmdate('U')-$timeStamp;

  if ($handle = opendir($dir)) {
    clearstatcache();
    while (false !== ($file = readdir($handle))) {
        if ($file != "." && $file != "..") {
          $files[] = $file;
        $index[] = filemtime( $dir.$file );
        }
    }
      closedir($handle);
  }
  asort( $index );

  foreach($index as $i => $t) {
    if($t < $time) {
      if (strpos($files[$i],'.') != 0){
        @unlink($dir.$files[$i]);
      }
    }

  }

}
?>