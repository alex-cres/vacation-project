<?php
global $user;
if (module_exists("event_calendar_holi_counter")) {
    _update_table_users2();
    $result = $query = db_select('event_calendar_days_counter', 'n')->fields('n')->condition('n.user_id', $user->uid)->addTag('')->execute()->fetchAssoc();
    echo t("Total in ")." ".date("Y").": " . (int)( (int) $result['counter_days'] +  (int) $result['counter_extra']);
    echo t("   Available: ").(int)( (int) $result['counter_days'] - (int) _measure_consumed_days2($user->uid, 1) );
     
   
}
function _measure_consumed_days2($id_user, $type)//1 used , 0 all execpt draft and changed and changed delete, 2 all submited only, 3 all approved only , 4 all draft
{
    $days_consumed = 0;
    $query_holi    = db_select('field_data_event_calendar_status', 'ecs');
    $query_holi->join('taxonomy_term_data', 'td', 'td.tid = ecs.event_calendar_status_tid');
    $query_holi->join('field_data_event_calendar_date', 'ecs2', 'ecs2.entity_id = ecs.entity_id');
    $query_holi->fields('ecs2', array(
        'event_calendar_date_value'
    ))->condition('td.name', 'national holiday');
    $result    = $query_holi->execute();
    $holidays  = array(); //removing holidays
    $holidays2 = array(); //for already set events to remove duplicates
    foreach ($result as $nod) {
        $holidays[] = $nod->event_calendar_date_value;
    }
    $query2 = db_select('node', 'n');
    $query2->join('field_data_event_calendar_date', 'ecs', 'ecs.entity_id=n.nid');
    $query2->join('field_data_event_calendar_status', 'ecs2', 'ecs2.entity_id=n.nid');
    
    $query2->fields('n')->fields('ecs')
	->condition('n.uid', $id_user)->condition('n.type', "event_calendar")
	->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("changed delete", "event_calendar_status"), "<>")
	->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("important day", "event_calendar_status"), "<>")
	->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("national holiday", "event_calendar_status"), "<>");
    
    if ($type == 0) {
        $query2->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("changed", "event_calendar_status"), "<>")->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("draft", "event_calendar_status"), "<>");
        
    }elseif ($type == 2) {
        $query2->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("changed", "event_calendar_status"), "<>")
		->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("draft", "event_calendar_status"), "<>")
		->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("approved", "event_calendar_status"), "<>");
        
    }elseif ($type == 3) {
        $query2->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("changed", "event_calendar_status"), "<>")
		->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("draft", "event_calendar_status"), "<>")
		->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("submitted", "event_calendar_status"), "<>");
        
    }elseif ($type == 4) {
        $query2->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("changed", "event_calendar_status"), "<>")
		->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("submitted", "event_calendar_status"), "<>")
        ->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("approved", "event_calendar_status"), "<>");
       
    }
    
    
    $query = $query2->execute();
    
    foreach ($query as $result) {
        $mark = FALSE;
        if (!($type == 0)) {
            $query2 = db_select('event_calendar_changed', 'n');
            $nor    = $query2->fields("n")->condition('n.node_original_id', (int) $result->nid, "=")->execute();
            while ($amb = $nor->fetchAssoc()) {
                if (isset($amb['node_original_id'])) {
                    $mark = TRUE;
                }
            }
            $query2 = db_select('event_calendar_changed_delete', 'n');
            $nor2   = $query2->fields("n")->condition('n.node_original_id', (int) $result->nid)->execute();
            while ($amb2 = $nor2->fetchAssoc()) {
                if (isset($amb2['node_original_id'])) {
                    $mark = TRUE;
                }
            }
        }
        
        
        $start = new DateTime($result->event_calendar_date_value);
        $start->sub(new DateInterval('PT' . $start->format("H") . 'H' . $start->format("i") . 'M' . $start->format("s") . 'S'));
        $end = new DateTime($result->event_calendar_date_value2);
        $end->sub(new DateInterval('PT' . $end->format("H") . 'H' . $end->format("i") . 'M' . $end->format("s") . 'S'));
        
        // otherwise the  end date is excluded (bug?)
        $end->modify('+1 day');
        $interval = $end->diff($start);
        // total days
        $days     = $interval->days;
        
        // create an iterateable period of date (P1D equates to 1 day)
        $period = new DatePeriod($start, new DateInterval('P1D'), $end);
        // best stored as array, so you can add more than one
        if (!$mark) {
            foreach ($period as $dt) {
                $dan  = false; //used to see if a holiday is met so it doesn't subtract a holiday in a sunday or saturday
                $dan2 = false;
                $curr = $dt->format('D');
                $year = $dt->format("Y");
                // for the updated question
                if ($year > date("Y") || $year < date("Y")) {
                    $days--;
                } else {
                    
                    foreach ($holidays as $noe) {
                        if ($dt->format("Y-m-d H:i:s") == $noe) {
                            $days--;
                            $dan = true;
                        }
                    }
                    // substract if Saturday or Sunday and not a holiday already
                    if (($curr == 'Sat' || $curr == 'Sun') && !($dan)) {
                        $days--;
                        $dan2 = true;
                    }
                    // substract if not Saturday or Sunday and not a holiday already and is overlaping another event
                    foreach ($holidays2 as $noe) {
                        if ($dt->format("Y-m-d H:i:s") == $noe && !($dan) && !($dan2)) {
                            $days--;
                            $dan = true;
                        }
                    }
                    
                    $holidays2[] = $dt->format("Y-m-d H:i:s");
                }
                
                
            }
        } else {
            $days = 0;
        }
        $days_consumed += $days;
    }
    return $days_consumed;
}
function _get_term_from_name2($term_name, $vocabulary_name)
{
    if ($vocabulary = taxonomy_vocabulary_machine_name_load($vocabulary_name)) {
        $tree = taxonomy_get_tree($vocabulary->vid);
        foreach ($tree as $term) {
            if ($term->name == $term_name) {
                return $term->tid;
            }
        }
    }
    return FALSE;
}

function _update_table_users2()
{
    $today_year = date("Y");
    $user       = array();
    $user_id    = array();
    $query2     = db_select('users', 'n')->fields('n')->condition("n.uid", '0', '<>')->addTag('')->execute();
    while ($result2 = $query2->fetchAssoc()) {
        
        $user[$result2['uid']]    = $result2['name'];
        $user_id[$result2['uid']] = $result2['uid'];
        
        $query = db_select('event_calendar_days_counter', 'n')->fields('n')->condition('n.counter_year', $today_year)->addTag('')->execute();
        while ($result = $query->fetchAssoc()) {
            if ($user_id[$result2['uid']] == $result['user_id']) {
                unset($user_id[$result2['uid']]);
                unset($user[$result2['uid']]);
                break;
            }
        }
    }
    foreach ($user_id as $nigg) {
       $query = db_select('event_calendar_days_counter', 'n')->fields('n')->condition('n.counter_year', $today_year -1)->addTag('')->execute();
        while ($result = $query->fetchAssoc()) {
			if ($nigg == $result['user_id']) {
				$days_extra=$result['counter_days'] + $result['counter_extra'] -$result['counter_days_used'];
		
			}
		}
        $nid = db_insert('event_calendar_days_counter') // Table name no longer needs {}
            ->fields(array(
            'user_id' => $nigg,
            'user_name' => $user[$nigg],
            'counter_days' => 22,
            'counter_year' => $today_year,
            'counter_days_used' => _measure_consumed_days2($nigg, 0),
            'counter_days_used_last' => 0,
			'counter_extra' => (isset($days_extra))?$days_extra:0,
        ))->execute();
    }
    
    $query = db_select('event_calendar_days_counter', 'n')->fields('n', array(
        'user_id'
    ))->condition('n.counter_year', $today_year)->addTag('')->execute();
    while ($nigg = $query->fetchAssoc()) {
	
        $num_updated = db_update('event_calendar_days_counter') // Table name no longer needs {}
            ->fields(array(
            'counter_days_used' => _measure_consumed_days2($nigg, 0),
		
        ))->condition('user_id', $nigg, '=')->execute();
    }
    
    
}

?>