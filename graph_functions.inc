<?php
/**
 * Given a start time, end time, and interval return averages for the fields in $datas
 * that are not Id or Date
 *
 * @param $startDateTime integer
 * @param $endDateTime integer
 * @param $interval integer
 * @param $datas array
 * @param $fields array
 * @param $aggregateMethod string
 * @throws Exception
 * @return array
 */
function getResultsDataAvg($startDateTime, $endDateTime, $interval, $datas, $fields, $aggregateMethod) {
  $percentile = null;
  switch ($aggregateMethod) {
      case 'avg':
          // throw new Exception("Average not implemented", 1);
          break;
      case '25th':
          $percentile=25;
          break;
      case '50th':
          $percentile=50;
          break;
      case '75th':
          $percentile=75;
          break;
      case '90th':
          $percentile=90;
          break;
      case '95th':
          $percentile=95;
          break;
      case '99th':
          $percentile=99;
          break;

      default:
          throw new Exception("Unknown aggregate method: $aggregateMethod", 1);
          break;
  }

  $pointer = $startDateTime;
//  $id = $datas[0]['Id'];
  $data = array();

  while ($pointer < $endDateTime) {
    $entry = array();
    //    $entry['Id'] = $id;
    $entry['Date'] = $pointer;
    foreach ($fields as $field) {
      if($aggregateMethod == "avg") {
        $entry[$field] = getAverage($pointer, $interval, $datas, $field, $percentile);
      }else {
        $entry[$field] = getPercentile($pointer, $interval, $datas, $field, $percentile);
      }
    }
    $data[] = $entry;
    $pointer += $interval;
  }
  return $data;
}

function getPercentile($startAt, $increment, &$data, $field, $percentile) {
  if ($percentile < 0 OR $percentile > 100) {
      throw new Exception("$percentile have to be bigger than 0 and smaller than 100", 1);
  }
  $tmpData = array();

  foreach ($data as $r) {
    if ($r['Date'] > $startAt && ($r['Date'] < $startAt + $increment)) {
        if($r[$field] !== null) {
            $tmpData[] = $r[$field];
        }
    }
  }

  $numberOfElements = count($tmpData);

  sort($tmpData);

  if ($numberOfElements > 0) {
      $percentilePosition = ($numberOfElements-1)*$percentile/100;
      $n1 = (int)floor($percentilePosition);
      $n2 = (int)ceil($percentilePosition);
      $value = $tmpData[$n1] + ($tmpData[$n2] - $tmpData[$n1]) * $percentile/100;
      return $value;
  } else {
      return null;
  }
}


/**
 * Format data in the amchart specific format
 *
 * @param  $userId
 * @param  $jobIds
 * @param  $availFields
 * @param  $fields
 * @param  $start
 * @param  $end
 * @param  $percentile
 * @param  $trimAbove
 * @param  $adjustUsing
 * @param  $trimBelow
 * @param  $interval
 * @param  $aggregateMethod
 * @return string
 */
//function getDataAsAmChartLineXml($userId,
//                                 $jobIds,
//                                 $availFields,
//                                 $fields,
//                                 $start,
//                                 $end,
//                                 $percentile,
//                                 $trimAbove,
//                                 $adjustUsing,
//                                 $trimBelow,
//                                 $interval,
//                                 $aggregateMethod) {
//  $dbFields = "r.Date";
//  foreach ($availFields as $field) {
//    $dbFields .= ",r." . $field;
//  }
//  $gid = 1;
//
//  $xmlData = "";
//  $xmlData .= "<chart>";
//  $xmlData .= "\n";
//  $xmlData .= "<series>";
//  $xmlData .= "\n";
//  $seriesData = getSeriesDataForMultiJobs($userId, $jobIds, $start, $end, $interval);
//
//  foreach ($seriesData as $data) {
//    $xmlData .= "<value xid=\"" . $data['Date'] . "\">" . date('m/d H:i', $data['Date']) . "</value>";
//    $xmlData .= "\n";
//  }
//  $xmlData .= "</series>";
//  $xmlData .= "\n";
//  $xmlData .= "<graphs>";
//  $xmlData .= "\n";
//  $xmlData .= getChangeNotesAsAmLineChartXml( $userId, $start, $end );
//  $jobTable = Doctrine_Core::getTable('WPTJob');
//  foreach ($jobIds as $jobId) {
//    $job = $jobTable->find($jobId);
//    $jobName = $job['Label'];
//    $datas = getGraphData( $jobId, $start, $end, $percentile, $trimAbove, $adjustUsing, $trimBelow, $dbFields );
//
//    if ($interval > 1) {
//      $datas = getResultsDataAvg($start, $end, $interval, $datas, $fields, $aggregateMethod);
//    }
//
//    foreach ($fields as $key => $field) {
//      $xmlGraphData = "";
//      $hasData = false;
//      foreach ($datas as $data) {
//        if ($data[$field] == 0) {
//          continue;
//        } else {
//          $hasData = true;
//          $value = $data[$field] / 1000;
//          $edt=$data['Date']+$interval;
//          $xmlGraphData .= "<value bullet_size=\"4\" bullet=\"round\" xid=\"" . $data['Date']
//              . "\" url=\"listResults.php?currentPage=1&amp;filterField=WPTJob.Id&amp;filterValue=".$jobId
//              ."&amp;startDateTime=" . $data['Date']
//              ."&amp;endDateTime=".$edt."\">"
//              . number_format($value,3) . "</value>";
//          $xmlGraphData .= "\n";
//        }
//      }
//      if ($hasData) {
//        $xmlData .= "<graph title=\"" . $jobName . "\n" . $key . "\" gid=\"" . $gid++ . "\">";
//            $xmlData .= "\n";
//        $xmlData .= $xmlGraphData;
//        $xmlData .= "\n";
//        $xmlData .= "</graph>";
//        $xmlData .= "\n";
//      }
//    }
//  }
//  $xmlData .= "</graphs>";
//  $xmlData .= "\n";
//  $xmlData .= "</chart>";
//  return $xmlData;
//}


//function getDataAsAmChartScatterXml($userId,
//                                    $jobIds,
//                                    $availFields,
//                                    $fields,
//                                    $start,
//                                    $end,
//                                    $percentile,
//                                    $trimAbove,
//                                    $adjustUsing,
//                                    $trimBelow,
//                                    $interval,
//                                    $aggregateMethod) {
//  $dbFields = "r.Date";
//
//  foreach ($availFields as $field) {
//    $dbFields .= ",r." . $field;
//  }
//  $gid = 1;
//
//  $xmlData = "";
//  $xmlData .= "<chart>";
//  $xmlData .= "\n";
//  $xmlData .= "<graphs>";
//  $xmlData .= "\n";
//  $xmlData .= getChangeNotesAsAmScatterChartXml( $userId, $start, $end );
//  $jobTable = Doctrine_Core::getTable('WPTJob');
//  foreach ($jobIds as $jobId) {
//    $job = $jobTable->find($jobId);
//    $jobName = $job['Label'];
//    $datas = getGraphData( $jobId, $start, $end, $percentile, $trimAbove, $adjustUsing, $trimBelow, $dbFields );
//
//    if ($interval > 1) {
//      $datas = getResultsDataAvg($start, $end, $interval, $datas, $fields, $aggregateMethod);
//    }
//    foreach ($fields as $key => $field) {
//      $xmlGraphData = "";
//      $hasData = false;
//      foreach ($datas as $data) {
//        if ($data[$field] == 0) {
//          continue;
//        } else {
//          $hasData = 1;
//          $value = $data[$field] / 1000;
//          $edt=$data['Date']+$interval;
//
//          $xmlGraphData .= "<point x=\"" . date("Y-m-d H:i", $data['Date'])
//              . "\" url=\"listResults.php?currentPage=1&amp;filterField=WPTJob.Id&amp;filterValue=".$jobId
//              ."&amp;startDateTime=" . $data['Date']
//              ."&amp;endDateTime=".$edt
//              ."\" y=\"" . number_format($value,3). "\"></point>";
//          $xmlGraphData .= "\n";
//        }
//      }
//      if ($hasData) {
//        $xmlData .= "<graph width=\"0\" title=\"" . $jobName . "\n" . $key . "\" gid=\"" . $gid++ . "\">";
//        $xmlData .= "\n";
//        $xmlData .= $xmlGraphData;
//        $xmlData .= "\n";
//        $xmlData .= "</graph>";
//        $xmlData .= "\n";
//      }
//    }
//  }
//  $xmlData .= "</graphs>";
//  $xmlData .= "\n";
//  $xmlData .= "</chart>";
//  return $xmlData;
//}

/**
 * Scans through the results to get time stamps for each point, removing duplicates.
 * Works across multiple job ids
 *
 * @param  $user_id
 * @param  $jobIds
 * @param  $startDateTime
 * @param  $endDateTime
 * @param int $interval
 * @param bool $includeChangeNotes
 * @return array
 */
function getSeriesDataForMultiJobs($user_id, $jobIds, $startDateTime, $endDateTime, $interval = 0, $includeChangeNotes = true) {
  if ($interval == 1) {
    $q = Doctrine_Query::create()->select('r.Date, r.WPTJobId')->from('WPTResult r')
        ->whereIn('r.WPTJobId', $jobIds)
        ->andWhere('r.ValidationState < ?', 2)
//        ->andWhere('r.WPTJob.UserId = ?', $user_id)
        ->andWhere('r.AvgFirstViewFirstByte > 0')
//        ->andWhere('r.AvgFirstViewStartRender > 0')
        ->andWhere('r.AvgFirstViewDocCompleteTime > 0')
        ->andWhere('r.AvgFirstViewFullyLoadedTime> 0')
        ->andWhere('r.Date <= ?', $endDateTime)
        ->andWhere('r.Date >= ?', $startDateTime)
        ->orderBy('r.Date')->distinct('r.Date')
        ->setHydrationMode(Doctrine_Core::HYDRATE_ARRAY);

    $result = $q->fetchArray();
    $q->free(true);
  } else {
    $pointer = $startDateTime;
    $result = array();

    while ($pointer < $endDateTime) {
      $entry = array();
      $entry['Date'] = $pointer;
      $pointer += $interval;
      $result[] = $entry;
    }
  }
  if ( $includeChangeNotes ){
    $changeNotes = getChangeNoteData( $user_id, $startDateTime, $endDateTime );
    foreach ($changeNotes as $note){
      $entry = array();
      $entry['Date'] = $note['Date'];
      $result[]=$entry;
    }
  }
  sort($result);
  return $result;
}

function getChangeNoteData( $user_id, $startDateTime, $endDateTime ) {
    $q = Doctrine_Query::create()->from('ChangeNote c')
        ->andWhere('c.Date <= ?', $endDateTime)
        ->andWhere('c.Date >= ?', $startDateTime)
        ->andWhere('c.Public')->orWhere('c.UserId = ?',$user_id)
        ->setHydrationMode(Doctrine_Core::HYDRATE_ARRAY);
    $result = $q->fetchArray();
    $q->free(true);
    return $result;
}

function getStatusesToNotInclude() {
    return array(-214702489, -214669720, -214669721, 100, 403, 404, 500, 501, 502, 503, 504, 910, 999, 99996, 99997, 99998);
}

function getGraphData( $job_id, $startDateTime, $endDateTime, $percentile = 1, $trimAbove = null, $adjustUsing, $trimBelow = null, $todStartHour, $todEndHour, $fields = null ) {
  $trimAbove = $trimAbove * 1000;
  $trimBelow = $trimBelow * 1000;

  if (!$fields) {
    $fields = "r.*";
  }
  $q = Doctrine_Query::create()->select($fields)->from('WPTResult r')
      ->where('r.ValidationState < ?', 2)
//      ->andWhere('r.WPTJob.UserId = ?', $user_id)
      // ->andWhere('r.AvgFirstViewFirstByte > 0')
//      ->andWhere('r.AvgFirstViewStartRender > 0')
      // ->andWhere('r.AvgFirstViewDocCompleteTime > 0')
      // ->andWhere('r.AvgFirstViewDocCompleteTime != ?', '')
//      ->andWhere('r.AvgFirstViewFullyLoadedTime > 0')
      ->andWhere('r.WPTJobId = ?', $job_id)
      ->andWhereNotIn('r.Status', getStatusesToNotInclude())
      ->andWhere('r.Date <= ?', $endDateTime)
      ->andWhere('r.Date >= ?', $startDateTime);
      // Including only working hours

    if($todStartHour < $todEndHour) {
        $q->andWhere('hour(from_unixtime(r.Date)) >= ? AND hour(from_unixtime(r.Date)) < ?', array($todStartHour, $todEndHour));
    } elseif ($todStartHour > $todEndHour) {
        $q->andWhere('hour(from_unixtime(r.Date)) >= ? OR hour(from_unixtime(r.Date)) < ?', array($todStartHour, $todEndHour));
    }

  if ($trimAbove) {
    $q->andWhere('r.' . $adjustUsing . ' < ?', $trimAbove);
  }
  if ($trimBelow) {
      $q->andWhere('r.' . $adjustUsing . ' > ?', $trimBelow);
  }
  if ($percentile < 1){
    $limit = $q->count() * $percentile;

    $q->orderBy('r.' . $adjustUsing)
      ->limit($limit)
      ->setHydrationMode(Doctrine_Core::HYDRATE_ARRAY);
    $res = $q->fetchArray();
    if (sizeof($res) > 0){
      $maxTime = $res[sizeof($res) - 1][$adjustUsing];
    }
    if ( isset($maxTime)){
      $q->andWhere('r.' . $adjustUsing . '< ?', $maxTime);
    }
  }
  $q->orderBy('r.Date')->setHydrationMode(Doctrine_Core::HYDRATE_ARRAY);

  $result = $q->fetchArray();
  $q->free(true);
  return $result;
}

/**
 * @param  $label
 * @param  $startDateTime
 * @param  $endDateTime
 * @param  $resolution
 * @param  $result
 * @return Array of averages of the data between startDateTime and endDateTime incrementing by resolution
 */
function getResultsData($label, $startDateTime, $endDateTime, $resolution, $result) {
  $pointer = $startDateTime;
  $samples = array();
  $comma = false;
  while ($pointer < $endDateTime) {
    $samples[$pointer] = getAverage($pointer, $resolution, $result, $label);
    if ($samples[$pointer] > 0) {
      if ($comma) {
        echo ",";
      }
      echo "[" . $pointer . "," . $samples[$pointer] . "]";
      if (!$comma) {
        $comma = true;
      }
    }
    $pointer += $resolution;
  }
  return $samples;
}

function getAverage($startAt, $increment, $data, $field) {
  $total = 0;
  $count = 0;
  $countNotNull = 0;

  foreach ($data as $r) {
    if ($r['Date'] > $startAt && ($r['Date'] < $startAt + $increment)) {
      if ($field == 'AvgFirstViewDocCompleteTime-AvgFirstViewDomTime') {
        $total = ($total + ($r['AvgFirstViewDocCompleteTime'] - $r['AvgFirstViewDomTime']));
        $countNotNull++;
        $count++;
      } elseif($r[$field] !== null) {
        $total = ($total + $r[$field]);
        $countNotNull++;
        $count++;
      }
    }
  }
  if ($count > 0 && $countNotNull > 0) {
    $result = $total / $count;
  } else {
    $result = NULL;
  }
  return $result;
}
function getChangeNotesAsAmLineChartXml( $userId, $start, $end ) {
  $changeNotes = getChangeNoteData( $userId, $start, $end );
  $xmlData = '';
  $xmlData .= "<graph title=\"Change Notes\" gid=\"0\">";
  $xmlData .= "\n";

  foreach ($changeNotes as $data) {
    $value = 0;
    $xmlData .= "<value description=\"".$data['Label']."\" bullet_size=\"10\" bullet=\"square\" xid=\"" . $data['Date'] . "\" url=\"listChangeNotes.php?filterField=Id&amp;filterValue=" . $data['Id'] . "\">" . $value . "</value>";
    $xmlData .= "\n";
  }
  $xmlData .= "</graph>";
  $xmlData .= "\n";
  return $xmlData;
}
function getChangeNotesAsAmScatterChartXml( $userId, $start, $end ) {
  $xmlData ="";
  $changeNotes = getChangeNoteData( $userId, $start, $end );
  $xmlData .= "<graph title=\"Change Notes\" gid=\"0\">";
  $xmlData .= "\n";

  foreach ($changeNotes as $data) {
    $value = 0;
    $xmlData .= "<point bullet_size=\"10\" x=\"" . date('Y-m-d H:i',$data['Date']) . "\" url=\"listChangeNotes.php?filterField=Id&amp;filterValue=" . $data['Id'] . "\" y=\"" . $value . "\">".$data['Label']."</point>";
    $xmlData .= "\n";
  }

  $xmlData .= "</graph>";
  $xmlData .= "\n";
  return $xmlData;
}
