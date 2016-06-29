<?php 
 function event_calendar_holi_counter_manage_events(&$form, &$form_state){
	 $form['filter']              = manage_events_filter_form();
    // Submit callback to filter events.
    $form['#submit'][]           = 'manage_events_filter_form_submit';
    // Form elements to provide list of events.
    $form['admin_events']        = manage_events_admin_events();
    // Form elements to provide status explaination.
    $form['event_calendar_note'] = event_calendar_status_note();
	
	
}
function manage_events_filter_form(){
global $user;
    // Collect all terms to show in filter option.
   $terms=array();
           $query = db_select('event_calendar_chief_counter', 'n');
        $nids = $query->fields('n')->condition("n.chief_id",$user->uid,"=")->orderBy("user_id","asc")->execute();
        while ($node = $nids->fetchAssoc()) {
		
            $terms[$node['user_id']] = $node['user_name'];
				
        }
	
	// Creating filter form.
    $form['filters'] = array(
        '#type' => 'fieldset',
        '#title' => t('Show only Vacations of user'),
        '#theme' => 'exposed_filters__node'
    );
    if (!empty($_SESSION['event_overview_filter_manage_events'])) {
        // Find filter status.
         $status = db_select('event_calendar_chief_counter', 'n');
        $status = $status->fields('n',array("user_name"))->condition("n.user_id",$_SESSION['event_overview_filter_manage_events'],"=")->execute()->fetchField();
       
		
        // Extra field to show filter status.
		
        $form['filters']['status']['text'] = array(
            '#markup' => t('where user is ').$status,
            '#value' => t('Reset')
        );
    }
    $form['filters']['status']['filters']           = array(
        '#title' => t('User'),
        '#type' => 'select',
        '#options' => $terms
    );
    $form['filters']['status']['actions']           = array(
        '#type' => 'actions',
        '#attributes' => array(
            'class' => array(
                'container-inline'
            )
        )
    );
    $form['filters']['status']['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Filter')
    );
    if (!empty($_SESSION['event_overview_filter_manage_events'])) {
        $form['filters']['status']['actions']['reset'] = array(
            '#type' => 'submit',
            '#value' => t('Reset')
        );
    }
    return $form;
}
function manage_events_filter_form_submit($form, &$form_state)
{

    $filters = $form_state['values']['filters'];
    if (!empty($_SESSION['event_overview_filter_manage_events'])) {
        unset($_SESSION['event_overview_filter_manage_events']);
    }
    if ($filters) {
        $_SESSION['event_overview_filter_manage_events'] = $filters;
    }
}






function manage_events_admin_events(){
$admin_access = user_access('administer event status');
    $node_type    = variable_get('event_calendar_node_type', 'event_calendar');
    
    // Build the 'Update options' form.
    $form['options'] = array(
        '#type' => 'fieldset',
        '#title' => t('Update Status'),
        '#attributes' => array(
            'class' => array(
                'container-inline'
            )
        ),
        '#access' => $admin_access
    );
    $options         = array("approved","denied");
    // Unset 'any' option.
   
    // Sort options array.
    
    $form['options']['operation'] = array(
        '#type' => 'select',
        '#title' => t('Operation'),
        '#title_display' => 'invisible',
        '#options' => $options,
        '#default_value' => 'approved'
    );
    $form['options']['submit']    = array(
        '#type' => 'submit',
        '#value' => t('Update'),
        '#validate' => array(
            'event_admin_events_validate'
        ),
        '#submit' => array(
            'manage_events_admin_events_submit'
        )
    );
    
    
    // Build the sortable table header.
    $header = array(
        'title' => array(
            'data' => t('Title'),
            'field' => 'n.title'
        ),
        'start-date' => array(
            'data' => t('Start Date (y-m-d)'),
            'field' => 'event_calendar_date_value',
            'sort' => 'desc'
        ),
        'end-date' => array(
            'data' => t('End Date (y-m-d)'),
            'field' => 'event_calendar_date_value2',
            'sort' => 'desc'
        ),
        'created' => array(
            'data' => t('Created Date (y-m-d)'),
            'field' => 'n.created',
            'sort' => 'desc'
        ),
        'updated' => array(
            'data' => t('Last Change (y-m-d)'),
            'field' => 'n.changed',
            'sort' => 'desc'
        ),
        'status' => array(
            'data' => t('Status'),
            'field' => 'n.status'
        )
    );
    
    $query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort');
    $query->join('field_data_event_calendar_date', 'ecs2', 'ecs2.entity_id = n.nid');
    $query->join('field_data_event_calendar_status', 'ecs', 'ecs.entity_id = n.nid');
    
    manage_event_build_filter_query($query);
    
    $nids = $query->fields('n')->fields('ecs2')->condition('type', $node_type)
->condition('ecs.event_calendar_status_tid', _get_term_from_name2("submitted", "event_calendar_status"), '=')
	
	->limit(50)->orderByHeader($header)->addTag('node_access')->execute()->fetchCol();
    
    $nodes       = node_load_multiple($nids);
    // Prepare the list of event nodes.
    $languages   = language_list();
    $destination = drupal_get_destination();
    $options     = array();
    foreach ($nodes as $node) {
        // Collect events Dates.
        $dates  = event_calendar_dates($node);
        // Collect events status/
        $status = event_calendar_status($node);
        // Set flag for expired events.
        if (is_event_expired($node, 'load')) {
            $flag1 = t("<i>**");
            $flag2 = t("</i>");
        } else {
            $flag1 = "";
            $flag2 = "";
        }
        $langcode = '';
        if (function_exists('entity_language')) {
            $langcode = entity_language('node', $node);
        }
        
        $l_options           = $langcode != LANGUAGE_NONE && isset($languages[$langcode]) ? array(
            'language' => $languages[$langcode]
        ) : array();
        $options[$node->nid] = array(
            'title' => array(
                'data' => array(
                    '#type' => 'link',
                    '#title' => $node->title,
                    '#href' => 'node/' . $node->nid,
                    '#options' => $l_options,
                    '#suffix' => ' ' . theme('mark', array(
                        'type' => node_mark($node->nid, $node->changed)
                    ))
                )
            ),
            'start-date' => $dates['start_date'],
            'end-date' => $dates['end_date'],
            'updated' => $dates['changed'],
            'created' => $dates['created'],
            
            'status' => $flag1 . $status . $flag2
        );
    }
    
    $form['events'] = array(
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $options,
        '#empty' => t('No content available.')
    );
    
    $form['pager'] = array(
        '#markup' => theme('pager')
    );
    return $form;
}


function manage_event_build_filter_query(SelectQueryInterface $query)
{
    // Build query.
    $filter_data = isset($_SESSION['event_overview_filter_manage_events']) ? $_SESSION['event_overview_filter_manage_events'] : '';
    if ($filter_data) {
        $query->join('event_calendar_chief_counter', 'eccc', 'eccc.user_id = n.uid');
        $query->condition('eccc.user_id', $filter_data);
    }
    
}
function manage_events_admin_events_submit($form, &$form_state){


    $events = array_filter($form_state['values']['events']);
    // Find out the term id.
    $op     = $form_state['values']['operation'];
	
    global $user;
    $node_type = variable_get('event_calendar_node_type', 'event_calendar');
    // Update each selected event node with new taxonomy status.
    variable_set('node_submitted_check_' . $node_type, 1);
    
    foreach ($events as $nid) {
        $obj                                                    = node_load($nid);
        $field_lang_code                                        = field_language('node', $obj, 'event_calendar_date', LANGUAGE_NONE);
        if($op==0){
			$obj->event_calendar_status[$field_lang_code][0]['tid'] =  _get_term_from_name2("approved", "event_calendar_status");
			$node_save                                              = node_save($obj);
		}elseif($op==1){
			node_delete($obj->nid);
		}
    }
    variable_set('node_submitted_check_' . $node_type, 0);
    
    drupal_set_message(t('The status have been changed.'));
}