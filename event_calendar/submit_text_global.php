<?php
function _get_term_from_name3($term_name, $vocabulary_name)
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
$node_type = variable_get('event_calendar_node_type', 'event_calendar');
$destination = $_SERVER['HTTP_REFERER'];
$path_parts = pathinfo($destination);
$view = views_get_view("vacation_calendar", TRUE);
$views_page = views_get_page_view();

$path_parts['basename']=$view->display[$views_page->current_display]->display_options['path']; 
$_SESSION['path_page']= $path_parts['basename'];

global $user;
if (isset($_GET['Submit'])) {

		if(module_exists("event_calendar_holi_counter")){
		$text="";
		$query = db_select('event_calendar_chief_counter', 'n')->fields('n')->condition("n.user_id",$user->uid ,"=")->addTag('Execute_Calm_Wave/#Azaptia first')->execute();
        while ($result = $query->fetchAssoc()) {
		$text.=$result['chief_emails'];
		}
	
		$_GET['email']=(($text=="")?"":$text).(($_GET['email']=="")?"":",").$_GET['email'];
		}
		
    if (!($_GET['email'] == "")) {
        $emails      = explode(",", $_GET['email']);
        $email_error = FALSE;
        foreach ($emails as $rd) {
            if (!valid_email_address(trim($rd))) {
                $email_error = TRUE;
            }
            
        }
        if ($email_error) {
            $_SESSION['q'] = 'email_no_error';
			 
			  
			  drupal_goto($_SESSION['path_page']);
			
			
        } else {
		
            echo "
					<form action='#' method='get'>	
					<p>Do you wish to submit your alterations to " . $_GET['email'] . "?</p>
					<input type='hidden' value='" . $_GET['email'] . "' name='email'>
					<input type='submit' name='Submit2' value='OK'><input type='submit' name='Submit3' value='Cancel' >
					</form>	
				";
        }
    } else {
	   
			  $_SESSION['q'] = 'email_no';
			     $destination = $_SERVER['HTTP_REFERER'];
			 
			   $path_parts = pathinfo($destination);
			   $_SESSION['q3']=$path_parts;
        drupal_goto($_SESSION['path_page']);
    }
} elseif (isset($_GET['Cancel'])) {
    echo "
			<form action='#' method='get'>
			<p>Do you really wish to cancel all alterations?</p>
			<input type='submit' name='Cancel2' value='Yes'><input type='submit' name='Submit3' value='No' >
			</form>
		";
} elseif (isset($_GET['Submit2'])) {
     $transaction = db_transaction();

  try {
	$alter    = FALSE;
    $to       = $_GET['email'];
    $actions  = "Submitted:\n";
    $actions2 = "";
    //Created type
    variable_set('node_submitted_check_' . $node_type, 1);
    $query2 = db_select('node', 'n');
    $query2->join('field_data_event_calendar_date', "ecs", "ecs.entity_id=n.nid");
    $query2->join("field_data_event_calendar_status", "ecs2", "ecs2.entity_id=n.nid");
    $query2->fields("n")->fields("ecs")->condition("n.uid", $user->uid)->condition("n.type", "event_calendar")->condition("ecs2.event_calendar_status_tid", _get_term_from_name3("pending", "event_calendar_status"), "=")->addTag("");
    $query = $query2->execute();
    foreach ($query as $result) {
        $alter  = TRUE;
        $node_p = node_load($result->nid);
        $actions .= "New: Title:\"" . $node_p->title . "\" Date: " . $node_p->event_calendar_date['und'][0]['value'] . " to " . $node_p->event_calendar_date['und'][0]['value2'] . "\n";
        $actions2 .= "New: Title:\"" . $node_p->title . "\" Date: " . $node_p->event_calendar_date['und'][0]['value'] . " to " . $node_p->event_calendar_date['und'][0]['value2'] . "<br>";
        $node_p->event_calendar_status['und'][0]['tid'] = _get_term_from_name3("submitted", "event_calendar_status");
        node_save($node_p);
    }
    if ($alter) {
        $actions .= "\n";
        $actions2 .= "<br>";
    }
    //changed type
    $query2 = db_select('node', 'n');
    $query2->join('field_data_event_calendar_date', "ecs", "ecs.entity_id=n.nid");
    $query2->join("field_data_event_calendar_status", "ecs2", "ecs2.entity_id=n.nid");
    $query2->join("event_calendar_changed", "ecs3", "ecs3.node_id=n.nid");
    $query2->fields("n")->fields("ecs3")->condition("n.uid", $user->uid)->condition("n.type", "event_calendar")->condition("ecs2.event_calendar_status_tid", _get_term_from_name3("changed", "event_calendar_status"), "=")->addTag("");
    $query = $query2->execute();
    $space = FALSE;
    foreach ($query as $result) {
        $alter           = TRUE;
        $space           = TRUE;
        $node_p          = node_load($result->nid);
        $node_p_original = node_load($result->node_original_id);
        
        $actions .= "Changed: \n Old -> Title:\"" . $node_p_original->title . "\" Date: " . $node_p_original->event_calendar_date['und'][0]['value'] . " to " . $node_p_original->event_calendar_date['und'][0]['value2'] . "\n";
        $actions .= "New -> Title:\"" . $node_p->title . "\" Date: " . $node_p->event_calendar_date['und'][0]['value'] . " to " . $node_p->event_calendar_date['und'][0]['value2'] . "\n";
        $actions2 .= "Changed: <br> Old -> Title:\"" . $node_p_original->title . "\" Date: " . $node_p_original->event_calendar_date['und'][0]['value'] . " to " . $node_p_original->event_calendar_date['und'][0]['value2'] . "<br>";
        $actions2 .= "New -> Title:\"" . $node_p->title . "\" Date: " . $node_p->event_calendar_date['und'][0]['value'] . " to " . $node_p->event_calendar_date['und'][0]['value2'] . "<br>";
        
        $node_p->event_calendar_status['und'][0]['tid'] = _get_term_from_name3("submitted", "event_calendar_status");
        node_save($node_p);
        node_delete($result->node_original_id);
        $num_deleted = db_delete('event_calendar_changed')->condition('node_id', $result->nid)->condition('user_id', $user->uid)->execute();
    }
    if ($space) {
        $actions .= "\n";
        $actions2 .= "<br>";
    }
    //deleted type
    $query2 = db_select('node', 'n');
    $query2->join('field_data_event_calendar_date', "ecs", "ecs.entity_id=n.nid");
    $query2->join("field_data_event_calendar_status", "ecs2", "ecs2.entity_id=n.nid");
    $query2->join("event_calendar_changed_delete", "ecs3", "ecs3.node_id=n.nid");
    $query2->fields("n")->fields("ecs3")->condition("n.uid", $user->uid)->condition("n.type", "event_calendar")->condition("ecs2.event_calendar_status_tid", _get_term_from_name3("changed delete", "event_calendar_status"), "=")->addTag("");
    $query = $query2->execute();
    foreach ($query as $result) {
        $alter           = TRUE;
        $node_p          = node_load($result->nid);
        $node_p_original = node_load($result->node_original_id);
        $actions .= "Deleted: Title:\"" . $node_p_original->title . "\" Date: " . $node_p_original->event_calendar_date['und'][0]['value'] . " to " . $node_p_original->event_calendar_date['und'][0]['value2'] . "\n";
        $actions2 .= "Deleted: Title:\"" . $node_p_original->title . "\" Date: " . $node_p_original->event_calendar_date['und'][0]['value'] . " to " . $node_p_original->event_calendar_date['und'][0]['value2'] . "<br>";
        node_delete($result->nid);
        node_delete($result->node_original_id);
        $num_deleted = db_delete('event_calendar_changed_delete')->condition('node_id', $result->nid)->condition('user_id', $user->uid)->execute();
    }
    $from              = (string) variable_get('site_mail', '');
    $language          = language_default();
    $subject           = 'Submission from user: ' . $user->name;
    $params['subject'] = $subject;
    variable_set('node_submitted_check_' . $node_type, 0);
    if ($alter) {
        if (module_exists('event_calendar_holi_counter')) {
            if(module_exists('core_table_cms') && function_exists('core_table_cms_fill_cms_table')){
				 $detalhes=array("actions" => $actions2);
				 $responce = core_table_cms_fill_cms_table($user->uid,1,$detalhes);
				$request= $responce['request'];
				$id=$responce['id'];
				
			}else{
				$query = db_select('event_calendar_request_counter', 'n');
				$nids  = $query->fields('n')->condition('n.request_year_part', date("Y"))->orderBy("n.request_id_part", 'DESC')->range(0, 1)->addTag('node_access')->execute()->fetchAssoc();
				if (!isset($nids['request_id_part'])) {
					$id =  sprintf('%05d', 1);
				} else {
					$id = sprintf('%05d',$nids['request_id_part'] + 1);
				}
				 $request = variable_get('ticket_event_calendar') . '-' . date("Y") . "-" .  $id;
           
			}
            $nid     = db_insert('event_calendar_request_counter') // Table name no longer needs {}
                ->fields(array(
                'user_id' => $user->uid,
                'type_letter' => (module_exists('core_table_cms'))? 'CMS': variable_get('ticket_event_calendar'),
                'type_action' => "Submission",
                'request_id_part' => $id,
                'request_actions' => $actions2,
                'request_year_part' => date("Y"),
                'request_date' => date("Y-m-d H:i:s"),
                'request_id' => $request
            ))->execute();
            $body    = "By user: " . $user->name . "\n\n Ticket Number: " . $request . "\n" . $actions;
        } else {
            $body = "By user: " . $user->name . "\n\n" . $actions;
        }
		
        $params['body'] = $body;
        $num_updated    = db_update('event_calendar_days_counter')->fields(array(
            'counter_days_used_last' => _measure_consumed_days10($user->uid, 0)
        ))->condition('user_id', $user->uid, '=')->execute();
        $query2         = db_select('node', 'n');
        $query2->join('field_data_event_calendar_date', "ecs", "ecs.entity_id=n.nid");
        $query2->join("field_data_event_calendar_status", "ecs2", "ecs2.entity_id=n.nid");
        $query2->fields("n")->fields("ecs")->condition("n.uid", $user->uid)->condition("n.type", "event_calendar")->condition("ecs2.event_calendar_status_tid", _get_term_from_name3("important day", "event_calendar_status"), "<>")->condition("ecs2.event_calendar_status_tid", _get_term_from_name3("holiday", "event_calendar_status"), "<>")->condition("ecs2.event_calendar_status_tid", _get_term_from_name3("submitted", "event_calendar_status"), "<>")->condition("ecs2.event_calendar_status_tid", _get_term_from_name3("approved", "event_calendar_status"), "<>")->addTag("");
        $query = $query2->execute();
        foreach ($query as $result) {
            
            $num_deleted = db_delete('event_calendar_changed')->condition('node_id', $result->nid)->condition('user_id', $user->uid)->execute();
            $num_deleted = db_delete('event_calendar_changed_delete')->condition('node_id', $result->nid)->condition('user_id', $user->uid)->execute();
        }
        drupal_mail('event_calendar', 'admin', $to, $language, $params, $from, $send = TRUE);
        $_SESSION['q'] = "submit";
		  
		 $_SESSION['q2'] = $request;
		 	   
        drupal_goto($_SESSION['path_page']);
    } else {
        variable_set('node_submitted_check_' . $node_type, 0);
		   
			   $_SESSION['q'] = "submit_no";
			     
        drupal_goto($_SESSION['path_page']);
    }
	
	
  }
  catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('my_type', $e);
	$_SESSION['q'] = "error";
	      
			   drupal_goto($_SESSION['path_page']);
  }
} elseif (isset($_GET['Cancel2'])) {
    variable_set('node_cancel_check_' . $node_type, 1);
    $query2 = db_select('node', 'n');
    $query2->join('field_data_event_calendar_date', "ecs", "ecs.entity_id=n.nid");
    $query2->join("field_data_event_calendar_status", "ecs2", "ecs2.entity_id=n.nid");
    $query2->fields("n")->fields("ecs")->condition("n.uid", $user->uid)->condition("n.type", "event_calendar")->condition("ecs2.event_calendar_status_tid", _get_term_from_name3("important day", "event_calendar_status"), "<>")->condition("ecs2.event_calendar_status_tid", _get_term_from_name3("holiday", "event_calendar_status"), "<>")->condition("ecs2.event_calendar_status_tid", _get_term_from_name3("submitted", "event_calendar_status"), "<>")->condition("ecs2.event_calendar_status_tid", _get_term_from_name3("approved", "event_calendar_status"), "<>")->addTag("");
    $query = $query2->execute();
    foreach ($query as $result) {
        node_delete($result->nid);
        $num_deleted = db_delete('event_calendar_changed')->condition('node_id', $result->nid)->condition('user_id', $user->uid)->execute();
        $num_deleted = db_delete('event_calendar_changed_delete')->condition('node_id', $result->nid)->condition('user_id', $user->uid)->execute();
    }
    variable_set('node_cancel_check_' . $node_type, 0);
    $_SESSION['q'] = "discard";
	  
	    drupal_goto($_SESSION['path_page']);
} elseif (isset($_GET['Submit3'])) {
   
    drupal_goto($_SESSION['path_page']);
} else {
    if (isset($_SESSION['q'])) {
        if ($_SESSION['q'] == 'discard') {
            drupal_set_message("Discarted all alterations that weren't saved");
        } elseif ($_SESSION['q'] == 'submit') {
            drupal_set_message("Saved alterations '". $_SESSION['q2'] ."'" );
			unset($_SESSION['q2']);
        } elseif ($_SESSION['q'] == 'submit_no') {
            drupal_set_message("No alterations were made. ");
        } elseif ($_SESSION['q'] == 'email_no') {
            drupal_set_message("Empty Email.", "error");
        } elseif ($_SESSION['q'] == 'email_no_error') {
            drupal_set_message("One or more mails not valid.", "error");
        }elseif ($_SESSION['q'] == "error") {
            drupal_set_message("There was an error in submiting your information, retry later", "error");
        }
        
        unset($_SESSION['q']);
    }
	  
	
    echo '<form action="#" method="get">
			Email to submit*: <input type="text" name="email">
			<input type="submit" name="Submit" value="Submit" ><input type="submit" name="Cancel" value="Cancel" >
			*
			
		';
		if(module_exists("event_calendar_holi_counter")){
		 echo "If you have chiefs they will be added here too.";
		}
		echo 'For more than one email use the "," between them.</form>';
}
function _measure_consumed_days10($id_user, $type)
{
    $days_consumed = 0;
    $query_holi    = db_select('field_data_event_calendar_status', 'ecs');
    $query_holi->join('taxonomy_term_data', 'td', 'td.tid = ecs.event_calendar_status_tid');
    $query_holi->join('field_data_event_calendar_date', 'ecs2', 'ecs2.entity_id = ecs.entity_id');
    $query_holi->fields('ecs2', array(
        'event_calendar_date_value'
    ))->condition('td.name', 'holiday');
    $result    = $query_holi->execute();
    $holidays  = array(); //removing holidays
    $holidays2 = array(); //for already set events to remove duplicates
    foreach ($result as $nod) {
        $holidays[] = $nod->event_calendar_date_value;
    }
    $query2 = db_select('node', 'n');
    $query2->join('field_data_event_calendar_date', 'ecs', 'ecs.entity_id=n.nid');
    $query2->join('field_data_event_calendar_status', 'ecs2', 'ecs2.entity_id=n.nid');
    $query2->fields('n')->fields('ecs')->condition('n.uid', $id_user)->condition('n.type', "event_calendar")->condition('ecs2.event_calendar_status_tid', _get_term_from_name3("changed delete", "event_calendar_status"), "<>")->condition('ecs2.event_calendar_status_tid', _get_term_from_name3("important day", "event_calendar_status"), "<>")->condition('ecs2.event_calendar_status_tid', _get_term_from_name3("holiday", "event_calendar_status"), "<>");
    
    if ($type == 0) {
        $query2->condition('ecs2.event_calendar_status_tid', _get_term_from_name3("changed", "event_calendar_status"), "<>")->condition('ecs2.event_calendar_status_tid', _get_term_from_name3("pending", "event_calendar_status"), "<>");
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
        $period   = new DatePeriod($start, new DateInterval('P1D'), $end);
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
?>