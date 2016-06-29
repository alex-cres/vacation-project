<?php
/**
 * @file
 * Event user administration UI.
 */

/**
 * Form builder: Builds the event user administration overview.
 *
 *
 *
 */


/**
 *Menu callback: Event Calendar Holidays Configuration
 *
 *
 */
function event_calendar_holi_counter_content($form, $form_state)
{
    $admin_access = user_access('administer nodes');
    $node_type    = variable_get('event_calendar_node_type', 'event_calendar');
    $path         = current_path();
    // Build the 'Create options' form.
    _update_table_users();
    if (!isset($form_state['storage']['confirm'])) {
        
        // Build the sortable table header.
        $header = array(
            'user_id' => array(
                'data' => t('#'),
                'field' => 'n.counter_id'
            ),
            'user_name' => array(
                'data' => t('User'),
                'field' => 'n.user_id'
            ),
            'counter_days' => array(
                'data' => t('Holiday Days'),
                'field' => 'n.counter_days'
            ),
            'counter_year' => array(
                'data' => t('Year'),
                'field' => 'n.counter_year',
                'sort' => 'desc'
            ),
            'counter_days_used' => array(
                'data' => t('Holiday Days Used'),
                'field' => 'n.counter_days_used'
            ),
			'counter_extra'=> array(
                'data' => t('Days from last Year'),
                'field' => 'n.counter_extra'
            )
            
        );
        
        $query = db_select('event_calendar_days_counter', 'n')->extend('PagerDefault')->extend('TableSort');
        event_build_filter_query($query);
        
        $nids = $query->fields('n')->limit(50)->orderByHeader($header)->addTag('Execute_Calm_Wave/#Azaptia')->execute();
        
        $options = array();
        while ($node = $nids->fetchAssoc()) {
            $options[$node['counter_id']] = array(
                'user_id' => sprintf('%03d', $node['counter_id']),
                'user_name' => $node['user_name'],
                'counter_days' => $node['counter_days'],
                'counter_year' => $node['counter_year'],
                'counter_days_used' => $node['counter_days_used'],
				'counter_extra'=>$node['counter_extra'],
            );
        }
        $form['events'] = array(
            '#type' => 'tableselect',
            '#header' => $header,
            '#options' => $options,
            '#empty' => t('No content available.')
        );
        //event_admin_del('.$node['holiday_id'].')
        $form['pager']  = array(
            '#markup' => theme('pager')
        );
        
        $form['edit'] = array(
            '#type' => 'submit',
            '#value' => t('Edit Selected'),
            '#validate' => array(
                'event_admin_events_validate'
            ),
            '#submit' => array(
                'event_calendar_holidays_submit'
            )
            
        );
        
        
        
        return $form;
    } else {
        $events            = array_filter($_SESSION['our_variable']);
        $form['options10'] = array(
            '#type' => 'fieldset',
            '#title' => t('Edit Settings'),
            '#access' => $admin_access
        );
        foreach ($events as $nid) {
            
            $query = db_select('event_calendar_days_counter', 'n');
            
            $nids = $query->fields('n')->condition('counter_id', $nid, "=")->addTag('Execute_Calm_Wave/#Azaptia')->execute();
            $node = $nids->fetchAssoc();
            // Unset 'any' option.
            // Sort options array.
            
            $form['options10']['days' . $nid] = array(
                '#type' => 'textfield',
                '#title' => t("User " . $node['user_name'] . " days"),
                '#default_value' => $node['counter_days']
            );
        }
        $form['confirm'] = array(
            '#type' => 'submit',
            '#value' => t('Confirm'),
            '#submit' => array(
                'event_calendar_holidays_submit'
            )
            
        );
        $form['cancel']  = array(
            '#value' => t('Cancel')
            
        );
        
        
        
        return $form;
    }
    
    
}

/**
 * Callback: Function to collect available terms for event calendar.
 */
function event_available_terms()
{
    // Build query to fetch available terms.
    $vid      = db_query("SELECT vid FROM {taxonomy_vocabulary} WHERE machine_name = :machine_name", array(
        ':machine_name' => TAXONOMY_NAME
    ))->fetchField();
    // Get all terms of a vocabulary.
    $taxonomy = taxonomy_get_tree($vid);
    // Add an extra option 'any', will be show as filter option.
    $terms    = array(
        'any'
    );
    foreach ($taxonomy as $term) {
        $terms[$term->tid] = $term->name;
    }
    return $terms;
}

/**
 * Apply filters for event node administration filters based on session.
 *
 * @param string $query
 *   A Select Query to which the filters should be applied.
 */
function event_build_filter_query(SelectQueryInterface $query)
{
    // Build query.
    $filter_data = isset($_SESSION['event_overview_filter']) ? $_SESSION['event_overview_filter'] : '';
    if ($filter_data) {
        $query->join('field_data_event_calendar_status', 'ecs', 'ecs.entity_id = n.nid');
        $query->condition('ecs.event_calendar_status_tid', $filter_data);
    }
    
}

/**
 * Process result from event node administration filter form.
 */
function event_filter_form_submit($form, &$form_state)
{
    $filters = $form_state['values']['filters'];
    if (!empty($_SESSION['event_overview_filter'])) {
        unset($_SESSION['event_overview_filter']);
    }
    if ($filters) {
        $_SESSION['event_overview_filter'] = $filters;
    }
}

/**
 * Validate event_admin_events form submissions.
 *
 * Check if any nodes have been selected to perform the chosen
 * 'Update option' on.
 */
function event_admin_events_validate($form, &$form_state)
{
    // Error if there are no items to select.
    if (!is_array($form_state['values']['events']) || !count(array_filter($form_state['values']['events']))) {
        form_set_error('', t('No selected.'));
    }
}

function event_calendar_holidays_submit(&$form, &$form_state)
{
    // Filter out unchecked event nodes.
    if (!isset($form_state['storage']['confirm'])) {
        // not confirmed yet.
        $_SESSION['our_variable']         = $form_state['values']['events']; // add our field into a session variable to keep it around for later
        // NOTE: The variable is no longer in $form_state['values']['your-field'] by the time I'm printing out the final confirmation message. I'm
        // not sure session variables are the best solution, but this worked for me.
        $form_state['storage']['confirm'] = TRUE; // this will cause the form to be rebuilt, entering the confirm part of the form
        $form_state['rebuild']            = TRUE;
        
    }
    
    else {
        $events2 = array_filter($_SESSION['our_variable']);
        $events  = array_filter($form_state['values']);
        $i       = 0;
        
        foreach ($events2 as $ev) {
            if (is_numeric($ev)) {
                $text[$i]=array('id' => $ev,'days' => array_values($events)[$i]);
                $i++;
            }
        }
        $error = false;
        foreach ($text as $ev) {
            if (is_numeric($ev['days'])) {
                $num_updated = db_update('event_calendar_days_counter') // Table name no longer needs {}
                    ->fields(array(
                    'counter_days' => $ev['days']
                ))->condition('counter_id', $ev['id'], '=')->execute();
            } else {
                drupal_set_message("Form had non-numeric days \"" . $ev['days'] . "\" for " . "#" . $ev['id'], 'error');
                $error = true;
            }
        }
        if ($error) {
            drupal_set_message("The remaining Edits have been efectuated.");
        } else {
            drupal_set_message("All Edits have been efectuated.");
            
        }
        
        
        
    }
}

function _update_table_users()
{
    $today_year = date("Y");
    $user       = array();
    $user_id    = array();
    $query2     = db_select('users', 'n')->fields('n')->condition("n.uid", '0', '<>')->addTag('Execute_Calm_Wave/#Azaptia first second')->execute();
    while ($result2 = $query2->fetchAssoc()) {
        
        $user[$result2['uid']]    = $result2['name'];
        $user_id[$result2['uid']] = $result2['uid'];
        
        $query = db_select('event_calendar_days_counter', 'n')->fields('n')->condition('n.counter_year', $today_year)->addTag('Execute_Calm_Wave/#Azaptia first')->execute();
        while ($result = $query->fetchAssoc()) {
            if ($user_id[$result2['uid']] == $result['user_id']) {
                unset($user_id[$result2['uid']]);
                unset($user[$result2['uid']]);
                break;
            }
        }
    }
	$days_extra=array();
    foreach ($user_id as $nigg) {
        $query = db_select('event_calendar_days_counter', 'n')->fields('n')->condition('user_id', $nigg, '=')->condition('n.counter_year', $today_year -1)->addTag('Execute_Calm_Wave/#Azaptia first')->execute();
        while ($result = $query->fetchAssoc()) {
				$days_extra[$result['user_id']]=($result['counter_days'] + $result['counter_extra'] -$result['counter_days_used']);
				
		}
        $nid = db_insert('event_calendar_days_counter') // Table name no longer needs {}
            ->fields(array(
            'user_id' => $nigg,
            'user_name' => $user[$nigg],
            'counter_days' => 22,
            'counter_year' => $today_year,
            'counter_days_used' => _measure_consumed_days($nigg),
            'counter_days_used_last' => 0,
			'counter_extra' => (isset($days_extra[$nigg]))?$days_extra[$nigg]:0,
        ))->execute();
		
       
    }
    $days_extra2=array();
    $query = db_select('event_calendar_days_counter', 'n')->fields('n',array("user_id"))->condition('n.counter_year', $today_year)->addTag('Execute_Calm_Wave/#Azaptia first')->execute();
    //TODO:Put updating agent, view all holidays, pending vacations and accepted vacations, subtract them all.
     while ($nigg = $query->fetchAssoc()) {
		$query2 = db_select('event_calendar_days_counter', 'n')->fields('n')->condition('n.counter_year', $today_year -1)->addTag('Execute_Calm_Wave/#Azaptia first');
		$query2=$query2 ->execute();
		while ($result = $query2->fetchAssoc()) {
				$days_extra2[$result['user_id']]=($result['counter_days'] + $result['counter_extra'] -$result['counter_days_used']);
		$num_updated = db_update('event_calendar_days_counter') // Table name no longer needs {}
            ->fields(array(
            'counter_days_used' => _measure_consumed_days($result['user_id']),
			'counter_extra' => (isset($days_extra2[$result['user_id']]))?$days_extra2[$result['user_id']]:0,
        ))->condition('user_id', $result['user_id'], '=')->condition('counter_year', $today_year, '=')->execute();
		}
        
		
		
    }
    drupal_set_message("Updated Table");
    
}

function _measure_consumed_days($id_user)
{
    $days_consumed = 0;
    $query_holi    = db_select('field_data_event_calendar_status', 'ecs');
    $query_holi->join('taxonomy_term_data', 'td', 'td.tid = ecs.event_calendar_status_tid');
    $query_holi->join('field_data_event_calendar_date', 'ecs2', 'ecs2.entity_id = ecs.entity_id');
    $query_holi->fields('ecs2', array(
        'event_calendar_date_value'
    ))->condition('td.name', 'holiday');
    $result    = $query_holi->execute();
    $result    = $query_holi->execute();
    $holidays  = array(); //removing holidays
    $holidays2 = array(); //for already set events to remove duplicates
    
    foreach ($result as $nod) {
        $holidays[] = $nod->event_calendar_date_value;
    }
    $query2 = db_select('node', 'n');
    $query2->join('field_data_event_calendar_date', 'ecs', 'ecs.entity_id=n.nid');
    $query2->join('field_data_event_calendar_status', 'ecs2', 'ecs2.entity_id=n.nid');
    
    $query2->fields('n')->fields('ecs')->condition('n.uid', $id_user)->condition('n.type', "event_calendar")->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("important day", "event_calendar_status"), "<>")->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("holiday", "event_calendar_status"), "<>")->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("changed", "event_calendar_status"), "<>")->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("changed delete", "event_calendar_status"), "<>")->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("pending", "event_calendar_status"), "<>")->addTag('Execute_Calm_Wave/#Gazia');
    $query = $query2->execute();
    
    foreach ($query as $result) {
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

function event_calendar_holi_counter_user_content($form, $form_state)
{

    $admin_access = user_access('administer event status');
    $node_type    = variable_get('event_calendar_node_type', 'event_calendar');
    $path         = current_path();
    
    $options   = array();
    $options[] = 'any';
    $query     = db_select('event_calendar_request_counter', 'n');
    $nids      = $query->fields('n', array(
        'request_year_part'
    ))->orderBy("request_year_part", 'desc')->groupBy("request_year_part")->addTag('Execute_Calm_Wave/#Gororia')->execute();
    while ($node = $nids->fetchAssoc()) {
        $options[$node['request_year_part']] = $node['request_year_part'];
    }
    
    if (isset($_SESSION['request_year_part'])) {
        $date_year2 = $_SESSION['request_year_part'];
    } else {
        $date_year2 = "any";
    }
    
    
    $form['options']['operation'] = array(
        '#type' => 'select',
        '#title' => t('Filter  Year'),
        '#options' => $options,
        '#default_value' => $date_year2
    );
    $form['options']['submit']    = array(
        '#type' => 'submit',
        '#value' => t('Update'),
        '#submit' => array(
            'event_submit_user'
        )
    );
    
    
    
    $header =event_calendar_holi_counter_headers();
	$options=event_calendar_holi_counter_options($header);
    
   
    $form['events'] = array(
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $options,
        '#empty' => t('No content available.')
    );
    //event_admin_del('.$node['holiday_id'].')
    $form['pager']  = array(
        '#markup' => theme('pager')
    );
    return $form;
    
    
}

function event_submit_user($form, $form_state)
{
    $op = $form_state['values']['operation'];
    if ($op != 0) {
        $_SESSION['request_year_part'] = $op;
    } else {
        unset($_SESSION['request_year_part']);
    }
    $form_state['rebuild'] = TRUE;
}


function event_calendar_holi_counter_headers(){
	return array(
        'request_id' => array(
            'data' => t('Request ID'),
            'field' => 'n.request_id',
            'sort' => 'desc'
        ),
        'user_name' => array(
            'data' => t('User'),
            'field' => 'u.name'
        ),
        'request_date' => array(
            'data' => t('Date'),
            'field' => 'n.request_id',
            'sort' => 'desc'
        ),
        'type_action' => array(
            'data' => t('Action Type'),
            'field' => 'n.type_action'
        ),
        'request_actions' => array(
            'data' => t('Actions'),
            'field' => 'n.request_actions'
        )
        
    );
	}
	
	function event_calendar_holi_counter_options($header){
	 $query = db_select('event_calendar_request_counter', 'n')->extend('PagerDefault')->extend('TableSort');
    if (isset($_SESSION['request_year_part'])) {
        $date_year = $_SESSION['request_year_part'];
    } else {
        $date_year = date("Y");
    }
    $query->join('users', 'u', 'u.uid = n.user_id');
    $query->fields('n')->fields('u');
    
    if (isset($_SESSION['request_year_part'])) {
        $query->condition('request_year_part', $_SESSION['request_year_part']);
    }
    
    $nids    = $query->limit(50)->orderByHeader($header)->addTag('Execute_Calm_Wave/#Gororia')->execute();
    $options = array();
    while ($node = $nids->fetchAssoc()) {
        $options[$node['request_id']] = array(
            'request_id' => $node['request_id'],
            'user_name' => $node['name'],
            'type_action' => $node['type_action'],
            'request_actions' => t($node['request_actions']),
            'request_date' => $node['request_date']
        );
    }
	return $options;
}



function event_calendar_holi_counter_content_chief($form, $form_state){

$admin_access = user_access('administer event status');
    $node_type    = variable_get('event_calendar_node_type', 'event_calendar');
    $path         = current_path();
    if (!isset($form_state['storage']['confirm'])) {
        _update_table_chiefs();
        // Build the sortable table header.
        $header = array(
            'user_id' => array(
                'data' => t('#'),
                'field' => 'n.user_id'
            ),
            'user_name' => array(
                'data' => t('User'),
                'field' => 'n.user_name'
            ),
           
           
            
			'chief_name'=> array(
                'data' => t('Chief'),
                'field' => 'u.name'
            )
            
        );
        
        $query = db_select('event_calendar_chief_counter', 'n')->extend('PagerDefault')->extend('TableSort');
		$query->join('users', 'u', 'u.uid = n.chief_id');
        $nids = $query->fields('n')->fields('u')->limit(50)->orderByHeader($header)->addTag('Execute_Calm_Wave/#Azaptia')->execute();
        
       $options=array();
        while ($node = $nids->fetchAssoc()) {
		
            $options[$node['user_id']] = array(
                'user_id' => sprintf('%03d', $node['user_id']),
                'user_name' => $node['user_name'],
				'chief_name'=> $node['name'],
            );
        }
        $form['events'] = array(
            '#type' => 'tableselect',
            '#header' => $header,
            '#options' => $options,
            '#empty' => t('No content available.')
        );
        //event_admin_del('.$node['holiday_id'].')
        $form['pager']  = array(
            '#markup' => theme('pager')
        );
        
        $form['edit'] = array(
            '#type' => 'submit',
            '#value' => t('Edit Selected'),
            '#validate' => array(
                'event_admin_events_validate'
            ),
            '#submit' => array(
                'event_calendar_chief_submit'
            )
            
        );
        
        
        
        return $form;
    } else {
        $events            = array_filter($_SESSION['our_variable']);
        $form['options10'] = array(
            '#type' => 'fieldset',
            '#title' => t('Edit Settings'),
            '#access' => $admin_access
        );
		
		 $options = array();
		$query = db_select('users', 'n')->fields('n')->condition("n.uid",0,"<>")->orderBy("n.uid","Asc")->execute();
		while($result=$query->fetchAssoc()){
		$options[$result['uid']]=$result['name'];
		}
		foreach ($events as $nid) {
            
            $query = db_select('event_calendar_chief_counter', 'n');
            
            $nids = $query->fields('n')->condition('user_id', $nid, "=")->addTag('Execute_Calm_Wave/#Azaptia')->execute();
            $node = $nids->fetchAssoc();
            // Unset 'any' option.
            // Sort options array.
             $form['options10'][$nid]=array(
				'#type' => 'fieldset',
				'#title' => t('User '). $node['user_name'],
				'#collapsible' =>TRUE
			 );
            $form['options10'][$nid]['days' . $nid] = array(
                '#type' => 'select',
                '#title' => t("Chief"),
				'#options' => $options,
                '#default_value' =>  $node['chief_id']
            );
        }
        $form['confirm'] = array(
            '#type' => 'submit',
            '#value' => t('Confirm'),
            '#submit' => array(
                'event_calendar_chief_submit'
            )
            
        );
        $form['cancel']  = array(
            '#value' => t('Cancel')
            
        );
        
        
        
        return $form;
    }
    
    
}





function event_calendar_chief_submit(&$form, &$form_state)
{
    // Filter out unchecked event nodes.
	
    if (!isset($form_state['storage']['confirm'])) {
        // not confirmed yet.
        $_SESSION['our_variable']         = $form_state['values']['events']; // add our field into a session variable to keep it around for later
        // NOTE: The variable is no longer in $form_state['values']['your-field'] by the time I'm printing out the final confirmation message. I'm
        // not sure session variables are the best solution, but this worked for me.
        $form_state['storage']['confirm'] = TRUE; // this will cause the form to be rebuilt, entering the confirm part of the form
        $form_state['rebuild']            = TRUE;
        
    }
    
    else {
        $events2 = array_filter($_SESSION['our_variable']);
        $events  = array_filter($form_state['values']);
		
        $i       = 0;
        
        foreach ($events2 as $ev) {
		
            if (is_numeric($ev)) {
                $text[$i]=array('id' => $ev,'days' => $events['days'.$ev]);
                $i++;
				
            }
        }
		
      
        foreach ($text as $ev) {
            
                $num_updated = db_update('event_calendar_chief_counter') // Table name no longer needs {}
                    ->fields(array(
                    'chief_id' => $ev['days']
                ))->condition('user_id', $ev['id'], '=')->execute();		
        }
	
            drupal_set_message("All Edits have been efectuated.");
    }
	
	
	
}

//todo:pass this when submiting
 function _update_table_chiefs(){
	$update=FALSE;
    $user       = array();
    $user_id    = array();
    $query2     = db_select('users', 'n')->fields('n')->condition("n.uid", '0', '<>')->addTag('Execute_Calm_Wave/#Azaptia first second')->execute();
    while ($result2 = $query2->fetchAssoc()) {
        
        $user[$result2['uid']]    = $result2['name'];
        $user_id[$result2['uid']] = $result2['uid'];
        
        $query = db_select('event_calendar_chief_counter', 'n')->fields('n')->addTag('Execute_Calm_Wave/#Azaptia first')->execute();
        while ($result = $query->fetchAssoc()) {
		    if ($user_id[$result2['uid']] == $result['user_id']) {
			
                unset($user_id[$result2['uid']]);
                unset($user[$result2['uid']]);
                break;
            }
        }
    }
    foreach ($user_id as $nigg) {
        $query = db_select('event_calendar_chief_counter', 'n')->fields('n')->condition('user_id', $nigg, '=')->addTag('Execute_Calm_Wave/#Azaptia first')->execute();
        
        $nid = db_insert('event_calendar_chief_counter') // Table name no longer needs {}
            ->fields(array(
            'user_id' => $nigg,
            'user_name' => $user[$nigg],
			'chief_id' => 1,
        ))->execute();
		
       $update=TRUE;
    }
    if($update){
	    drupal_set_message("Updated Table");
    
	}
    
 }
