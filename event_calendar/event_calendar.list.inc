<?php

/**
 * @file
 * Event administration UI.
 */

/**
 * Form builder: Builds the event administration overview.
 *
 * @see event_available_terms()
 * @see event_build_filter_query()
 * @see event_calendar_dates()
 * @see event_calendar_status()
 */



/**
 * Helper function to dynamically get the tid from the term_name
 *
 * @param $term_name Term name
 * @param $vocabulary_name Name of the vocabulary to search the term in
 *
 * @return Term id of the found term or else FALSE
 */
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
function getHolidays($country)
{
    //Url of Site with list
    $url     = 'http://www.timeanddate.com/holidays/' . $country . '/';
    //Use curl to get the page
    $ch      = curl_init();
    $timeout = 5;
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, $timeout);
    $html = curl_exec($ch);
    curl_close($ch);
    $dom = new DOMDocument();
    # The @ before the method call suppresses any warnings that
    # loadHTML might throw because of invalid HTML in the page.
    @$dom->loadHTML($html);
    $holidays = array();
    $items    = $dom->getElementsByTagName('tr');
    function tdrows($elements)
    {
        $str = "";
        foreach ($elements as $element) {
            $str .= $element->nodeValue . ", ";
        }
        //This pplaces the items into an array 
        $tempArray = explode(',', $str);
        //This gets rid of empty array elements
        unset($tempArray[4]);
        unset($tempArray[5]);
        return $tempArray;
    }
    foreach ($items as $node) {
        $holidays[] = tdrows($node->childNodes);
    }
    //The first and second items in the array were the titles of the table and a blank row 
    //so we unset them
    unset($holidays[0]);
    unset($holidays[1]);
    //then re index the array
    $holidays = array_values($holidays);
    return $holidays;
}
/**
 * Function to create Portuguese  holiday nodes for the current year
 *
 * TODO: add other countries' holidays if need be
 */
function _create_holiday_nodes()
{
    $country    = 'portugal';
    $holiday    = array();
    $node_type  = variable_get('event_calendar_node_type', 'event_calendar');
    $days       = array();
    $today_year = date("Y");
    $text       = getHolidays($country);
    foreach ($text as $day) {
        $dayfinder = date('U', strtotime($day[0] . " " . $today_year));
		 preg_match("/Season/", $day[3], $output_array);
		if(!isset($output_array[0])){
			preg_match("/Clock change/", $day[3], $output_array);
			if(!isset($output_array[0])){
			
				preg_match("/National holiday/", $day[3], $output_array);
				if (isset($output_array[0])) {
					$holiday[] = array(
						"name" => $day[2],
						"date" => date("Y-m-d H:i:s", $dayfinder)
					);
				
				} else {
					$days[] = array(
						"name" => $day[2],
						"date" => date("Y-m-d H:i:s", $dayfinder)
					);
					
				}
			}
		}
    }
    
    
    /*
    
    //Easter Calculus 
    $a=$today_year%19;
    $b=floor($today_year/100);
    $c=$today_year%100;
    $d=floor($b/4);
    $e=$b%4;
    $f=floor(($b+8)/25);
    $g=floor(($b-$f+1)/3);
    $h=(19*$a+$b-$d-$g+15)%30;
    $i2=floor($c/4);
    $k=$c%4;
    $l=(32+2*$e+2*$i2-$h-$k)%7;
    $m=floor(($a+11*$h+22*$l)/451);
    $Easter_Month =floor(($h+$l-7*$m+114)/31);  //[3=March, 4=April]
    $p=($h+$l-7*$m+114)%31;
    $Easter_Day=$p+1;
    $Easter=date("Y-m-d H:i:s",mktime(0,0,0,$Easter_Month,$Easter_Day,($today_year)));
    $dateGoodFriday=date_create($Easter);
    date_sub($dateGoodFriday, date_interval_create_from_date_string('02 days'));
    $dateCarnival=date_create($Easter);
    date_sub($dateCarnival, date_interval_create_from_date_string('47 days'));
    $dateCorpus=date_create($Easter);
    date_add($dateCorpus, date_interval_create_from_date_string('60 days'));
    
    $holiday[]=	array("name" => "Ano novo, Solenidade de Santa Maria","date"=> date("Y-m-d H:i:s",mktime(0,0,0,1,1,($today_year))));
    $holiday[]=	array("name" => "Dia da Liberdade","date"=> date("Y-m-d H:i:s",mktime(0,0,0,4,25,($today_year))));
    $holiday[]=	array("name" => "Dia do Trabalhador","date"=> date("Y-m-d H:i:s",mktime(0,0,0,5,1,($today_year))));  
    $holiday[]=	array("name" => "Dia de Portugal, de Camões e das Comunidades Portuguesas","date"=> date("Y-m-d H:i:s",mktime(0,0,0,6,10,($today_year))));  
    $holiday[]=	array("name" => "Assunção de Nossa Senhora","date"=> date("Y-m-d H:i:s",mktime(0,0,0,8,15,($today_year))));  
    $holiday[]=	array("name" => "Implantação da República","date"=> date("Y-m-d H:i:s",mktime(0,0,0,10,5,($today_year)))); 
    $holiday[]=	array("name" => "Dia de Todos-os-Santos","date"=> date("Y-m-d H:i:s",mktime(0,0,0,11,1,($today_year))));
    $holiday[]=	array("name" => "Restauração da Independência","date"=> date("Y-m-d H:i:s",mktime(0,0,0,12,1,($today_year))));  
    $holiday[]=	array("name" => "Imaculada Conceição","date"=> date("Y-m-d H:i:s",mktime(0,0,0,12,8,($today_year))));  
    $holiday[]=	array("name" => "Natal","date"=> date("Y-m-d H:i:s",mktime(0,0,0,12,25,($today_year))));
    $holiday[]=	array("name" => "Páscoa","date"=> $Easter);
    $holiday[]=	array("name" => "Sexta-feira Santa","date"=> date_format($dateGoodFriday,"Y-m-d H:i:s"));
    $holiday[]=	array("name" => "Corpo de Deus","date"=> date_format($dateCorpus,"Y-m-d H:i:s"));
    //Not a holiday, just a day
    $dayfinder=date('U', strtotime('first Sunday of May '.$today_year));
    $days[]=array("name" => "Dia da Mãe","date"=>date("Y-m-d H:i:s",$dayfinder));
    $days[]=array("name" => "Dia de São Valentin","date"=>date("Y-m-d H:i:s",mktime(0,0,0,2,14,($today_year))));
    $days[]=array("name" => "Dia do Pai","date"=>date("Y-m-d H:i:s",mktime(0,0,0,3,19,($today_year))));
    $days[]=array("name" => "Dia internacional da Mulher","date"=>date("Y-m-d H:i:s",mktime(0,0,0,3,8,($today_year))));
    $dayfinder2=date('U', strtotime('last Sunday of March '.$today_year));
    $days[]=array("name" => "Horário de Verão","date"=>date("Y-m-d H:i:s",$dayfinder2));
    $dayfinder3=date('U', strtotime('last Sunday of October '.$today_year));
    $days[]=array("name" => "Horário de Inverno","date"=>date("Y-m-d H:i:s",$dayfinder3));
    $days[]=array("name" => "Dia Mundial da Criança","date"=>date("Y-m-d H:i:s",mktime(0,0,0,6,1,($today_year))));
    $days[]=array("name" => "Último dia do Ano","date"=> date("Y-m-d H:i:s",mktime(0,0,0,12,31,($today_year))));
    $days[]=array("name" => "Véspera de Natal","date"=> date("Y-m-d H:i:s",mktime(0,0,0,12,24,($today_year))));
    $days[]=array("name" => "Carnaval","date"=> date_format($dateCarnival,"Y-m-d H:i:s"));
    */
    $query = db_select('event_calendar_holiday_custom', 'n');
    $nids  = $query->fields('n')->addTag('')->execute();
    while ($part = $nids->fetchAssoc()) {
        if ($part['holiday_type'] == "holiday") {
            if (date_format(date_create($part['holiday_date']), "Y") == $today_year) {
                $holiday[] = array(
                    "name" => $part['holiday_name'],
                    "date" => $part['holiday_date'],
                    "date2" => $part['holiday_date2']
                );
            } else {
                $day1     = date_create($part['holiday_date']);
                $day2     = date_create($part['holiday_date2']);
                $interval = abs((int) (date_format($day1, "Y")) - (int) ($today_year));
                date_add($day1, date_interval_create_from_date_string($interval . " years"));
                date_add($day2, date_interval_create_from_date_string($interval . " years"));
                $holiday[] = array(
                    "name" => $part['holiday_name'],
                    "date" => date_format($day1, "Y-m-d H:i:s"),
                    "date2" => date_format($day2, "Y-m-d H:i:s")
                );
            }
        } elseif ($part['holiday_type'] == "important day") {
            if (date_format(date_create($part['holiday_date']), "Y")) {
                $days[] = array(
                    "name" => $part['holiday_name'],
                    "date" => $part['holiday_date'],
                    "date2" => $part['holiday_date2']
                );
            } else {
                $day1     = date_create($part['holiday_date']);
                $day2     = date_create($part['holiday_date2']);
                $interval = abs((int) (date_format($day1, "Y")) - (int) ($today_year));
                date_add($day1, date_interval_create_from_date_string($interval . " years"));
                date_add($day2, date_interval_create_from_date_string($interval . " years"));
                $days[] = array(
                    "name" => $part['holiday_name'],
                    "date" => date_format($day1, "Y-m-d H:i:s"),
                    "date2" => date_format($day2, "Y-m-d H:i:s")
                );
            }
        }
    }
    
    
    
    
    variable_set('node_submitted_check_' . $node_type, 1);
    
    $i = 0;
    while ($i < count($holiday)) {
        $node           = new stdClass(); // We create a new node object
        $node->type     = "event_calendar"; // Or any other content type you want
        $node->title    = t($holiday[$i]['name']);
        $node->language = LANGUAGE_NONE; // Or any language code if Locale module is enabled. More on this below *
        $node->uid      = 1; // Or any id you wish		
        node_object_prepare($node); // Set some default values.
        $node->event_calendar_date['und'][0]['value'] = $holiday[$i]['date']; //begin date
        if (isset($holiday[$i]['date2'])) {
            $node->event_calendar_date['und'][0]['value2'] = $holiday[$i]['date2']; //end date
        } else {
            $node->event_calendar_date['und'][0]['value2'] = $holiday[$i]['date']; //end date
        }
        $node->event_calendar_status['und'][0]['tid'] = _get_term_from_name2("holiday", "event_calendar_status"); //Holiday
        
        $node = node_submit($node); // Prepare node for a submit
        node_save($node); // After this call we'll get a nid  
        $i++;
    }
    $i = 0;
    while ($i < count($days)) {
        $node           = new stdClass(); // We create a new node object
        $node->type     = "event_calendar"; // Or any other content type you want
        $node->title    = t($days[$i]['name']);
        $node->language = LANGUAGE_NONE; // Or any language code if Locale module is enabled. More on this below *
        $node->uid      = 1; // Or any id you wish
        node_object_prepare($node); // Set some default values.
        $node->event_calendar_date['und'][0]['value'] = $days[$i]['date']; //begin date
        if (isset($days[$i]['date2'])) {
            $node->event_calendar_date['und'][0]['value2'] = $days[$i]['date2']; //end date
        } else {
            $node->event_calendar_date['und'][0]['value2'] = $days[$i]['date']; //end date
        }
        $node->event_calendar_status['und'][0]['tid'] = _get_term_from_name2("important day", "event_calendar_status"); //important day
        
        node_save($node); // After this call we'll get a nid  
        $i++;
    }
    $node_type = variable_get('event_calendar_node_type', 'event_calendar');
    variable_set('node_submitted_check_' . $node_type, 0);
    if (count($holiday) != 0 && count($days) != 0) {
        drupal_set_message(t('The Holidays have been created'));
    } else {
        drupal_set_message(t('Internet connection failed or site information not available'));
    }
}

function event_admin_events()
{
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
    $options         = event_available_terms();
    // Unset 'any' option.
    unset($options['0']);
    // Sort options array.
    asort($options);
    $form['options']['operation'] = array(
        '#type' => 'select',
        '#title' => t('Operation'),
        '#title_display' => 'invisible',
        '#options' => $options,
        '#default_value' => 'approve'
    );
    $form['options']['submit']    = array(
        '#type' => 'submit',
        '#value' => t('Update'),
        '#validate' => array(
            'event_admin_events_validate'
        ),
        '#submit' => array(
            'event_admin_events_submit'
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
    
    event_build_filter_query($query);
    
    $nids = $query->fields('n')->fields('ecs2')->condition('type', $node_type)->condition('ecs.event_calendar_status_tid', _get_term_from_name2("changed", "event_calendar_status"), '<>')->condition('ecs.event_calendar_status_tid', _get_term_from_name2("changed delete", "event_calendar_status"), '<>')->condition('ecs.event_calendar_status_tid', _get_term_from_name2("pending", "event_calendar_status"), '<>')->limit(50)->orderByHeader($header)->addTag('node_access')->execute()->fetchCol();
    
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
/**
 *Menu callback: Event Calendar Holidays Configuration
 *
 *
 */
function event_calendar_holidays($form, $form_state)
{
    $admin_access = user_access('administer nodes');
    $node_type    = variable_get('event_calendar_node_type', 'event_calendar');
    $path         = current_path();
    // Build the 'Create options' form.
    if (!isset($form_state['storage']['confirm'])) {
	$form['leave'] = array(
            '#type' => 'link',
			'#title' => t('Go back'),
			'#href' =>'vacation_calendar/',
            
        );
        $form['options2'] = array(
            '#type' => 'fieldset',
            '#title' => t('Create Holidays for this Year'),
            '#attributes' => array(
                'class' => array(
                    'container-inline'
                )
            ),
            '#access' => $admin_access
        );
        $options          = event_available_terms();
        // Unset 'any' option.
        unset($options['0']);
        // Sort options array.
        asort($options);
        
        $form['options2']['submit2'] = array(
            '#type' => 'submit',
            '#value' => t('Create'),
            '#submit' => array(
                '_create_holiday_nodes'
            )
        );
        $form['options3']            = array(
            '#type' => 'fieldset',
            '#title' => t('Delete all holidays created'),
            '#attributes' => array(
                'class' => array(
                    'container-inline'
                )
            ),
            '#access' => $admin_access
        );
        
        $form['options3']['submit3'] = array(
            '#type' => 'submit',
            '#value' => t('Destroy'),
            '#submit' => array(
                '_delete_holiday_nodes'
            )
        );
        
        
        // Build the 'Actions options' form.
        $form['options'] = array(
            '#type' => 'fieldset',
            '#title' => t('Create new holiday'),
            '#access' => $admin_access
        );
        // Unset 'any' option.
        
        // Sort options array.
        
        $form['options']['name'] = array(
            '#type' => 'textfield',
            '#title' => t("Holiday Name"),
            '#attributes' => array(
                'class' => array(
                    'container-inline'
                )
            )
        );
        
        $form['options']['operation'] = array(
            '#type' => 'select',
            '#title' => t('Type'),
            '#options' => array(
                "holiday",
                "important day"
            ),
            '#default_value' => 'holiday',
            '#attributes' => array(
                'class' => array(
                    'container-inline'
                )
            )
        );
        $form['options']['dob']       = array(
            '#type' => 'date_popup',
            '#title' => t("Begin Date"),
            '#date_format' => 'Y-m-d',
            '#attributes' => array(
                'autocomplete' => 'off',
                'readonly' => 'readonly'
            )
        );
        
        $form['options']['dob2'] = array(
            '#type' => 'date_popup',
            '#title' => t("Ending Date"),
            '#date_format' => 'Y-m-d',
            '#attributes' => array(
                'autocomplete' => 'off',
                'readonly' => 'readonly'
            )
        );
        
        $form['options']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Add'),
            '#validate' => array(
                'event_admin_holi_validate'
            ),
            '#submit' => array(
                'event_admin_holi_submit'
            )
        );
        
        
        // Build the sortable table header.
        $header = array(
            'holiday_id' => array(
                'data' => t('#'),
                'field' => 'n.holiday_id'
            ),
            'holiday_name' => array(
                'data' => t('Title'),
                'field' => 'n.holiday_name'
            ),
            'start-date' => array(
                'data' => t('Start Date (y-m-d)'),
                'field' => 'holiday_date',
                'sort' => 'desc'
            ),
            'end-date' => array(
                'data' => t('End Date (y-m-d)'),
                'field' => 'holiday_date2',
                'sort' => 'desc'
            ),
            'holiday_type' => array(
                'data' => t('Type'),
                'field' => 'n.holiday_type',
                'sort' => 'desc'
            )
        );
        
        $query = db_select('event_calendar_holiday_custom', 'n')->extend('PagerDefault')->extend('TableSort');
        
        $nids = $query->fields('n')->limit(50)->orderByHeader($header)->addTag('')->execute();
        
        $options = array();
        while ($node = $nids->fetchAssoc()) {
            $options[$node['holiday_id']] = array(
                'holiday_id' => $node['holiday_id'],
                'holiday_name' => $node['holiday_name'],
                'start-date' => $node['holiday_date'],
                'end-date' => $node['holiday_date2'],
                'holiday_type' => $node['holiday_type']
            );
        }
        $form['events'] = array(
            '#type' => 'tableselect',
            '#header' => $header,
            '#options' => $options,
            '#empty' => t('No content available.')
        );
        $form['delete'] = array(
            '#type' => 'submit',
            '#value' => t('Delete Selected'),
            '#validate' => array(
                'event_admin_events_validate'
            ),
            '#submit' => array(
                'event_calendar_holidays_submit'
            )
            
        );
        //event_admin_del('.$node['holiday_id'].')
        $form['pager']  = array(
            '#markup' => theme('pager')
        );
        return $form;
    } else {
        $question = "The following will be destroyed: ";
        $events   = array_filter($_SESSION['our_variable']);
        foreach ($events as $nid) {
            $query = db_select('event_calendar_holiday_custom', 'n')->fields('n');
            $query->condition('n.holiday_id', $nid);
            $result = $query->execute()->fetchAssoc();
            
            $question .= "<br>" . $nid . "->" . $result['holiday_name'] . ": Type->" . $result['holiday_type'] . ",Begining Date->" . $result['holiday_date'] . ",Ending Date->" . $result['holiday_date2'];
        }
        $form['intro'] = array(
            '#markup' => $question
        );
        
        
        //.implode(',',$events).""
        return confirm_form($form, "Delete holidays", $path);
    }
    return $form;
}

/**
 * Menu callback: Events administration.
 */
function event_calendar_content($form, $form_state)
{
	
    // Form elements to provide filter options.
    $form['filter']              = event_filter_form();
    // Submit callback to filter events.
    $form['#submit'][]           = 'event_filter_form_submit';
    // Form elements to provide list of events.
    $form['admin_events']        = event_admin_events();
    // Form elements to provide status explaination.
    $form['event_calendar_note'] = event_calendar_status_note();
    if(module_exists("event_calendar_holi_counter")){
			$form=array();
			include("event_calendar_holi_counter/event_calendar_holi_counter_manager.inc");
			event_calendar_holi_counter_manage_events($form, $form_state);
		
	}
	return $form;
}

/**
 * Callback function Form element and form element explaining status state.
 */
function event_calendar_status_note()
{
    // From element to explain expired events.
    $form['event_status_note'] = array(
        '#type' => 'item',
        '#markup' => t('<i>** these events are expired.</i>')
    );
    return $form;
}

/**
 * Callback function events status,
 * If event is expired then return a custom status as 'expired'.
 */
function event_calendar_status($node)
{
    // Build query to fetch status of events.
    $query = db_select('field_data_event_calendar_status', 'ecs');
    $query->join('taxonomy_term_data', 'td', 'td.tid = ecs.event_calendar_status_tid');
    $query->fields('td', array(
        'name'
    ))->condition('ecs.entity_id', $node->nid);
    $result = $query->execute();
    foreach ($result as $status) {
        return $status->name;
    }
}


/**
 * Callback: function to collect events start date and end date.
 */
function event_calendar_dates($node)
{
    // Build query to fetch events dates.
    // Event start date.
    $dates           = array(
        NULL
    );
    $field_lang_code = field_language('node', $node, 'event_calendar_date', LANGUAGE_NONE);
    if (isset($node->event_calendar_date[$field_lang_code][0]['value'])) {
        $dates['start_date'] = $node->event_calendar_date[$field_lang_code][0]['value'];
        // Event end date.
    }
    if (isset($node->event_calendar_date[$field_lang_code][0]['value2'])) {
        $dates['end_date'] = $node->event_calendar_date[$field_lang_code][0]['value2'];
    }
    
    if (isset($node->created)) {
        $dates['created'] = gmdate("Y-m-d H:i:s", $node->created);
    }
    
    if (isset($node->changed)) {
        $dates['changed'] = gmdate("Y-m-d H:i:s", $node->changed);
    }
    return $dates;
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
        
        if ($term->tid == _get_term_from_name2("changed", "event_calendar_status")) {
            unset($terms[$term->tid]);
        }
        if ($term->tid == _get_term_from_name2("changed delete", "event_calendar_status")) {
            unset($terms[$term->tid]);
        }
        if ($term->tid == _get_term_from_name2("pending", "event_calendar_status")) {
            unset($terms[$term->tid]);
        }
    }
    return $terms;
}

/**
 * Return form for events node administration filters.
 */
function event_filter_form()
{
    // Collect all terms to show in filter option.
    $terms = event_available_terms();
    asort($terms);
    // Creating filter form.
    $form['filters'] = array(
        '#type' => 'fieldset',
        '#title' => t('Show only Vacations where'),
        '#theme' => 'exposed_filters__node'
    );
    if (!empty($_SESSION['event_overview_filter'])) {
        // Find filter status.
        $status                            = db_query("SELECT name FROM {taxonomy_term_data} WHERE tid = :tid", array(
            ':tid' => $_SESSION['event_overview_filter']
        ))->fetchField();
        // Extra field to show filter status.
        $form['filters']['status']['text'] = array(
            '#markup' => t('where status is <b> %status </b>', array(
                '%status' => $status
            )),
            '#value' => t('Reset')
        );
    }
    $form['filters']['status']['filters']           = array(
        '#title' => t('status'),
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
    if (!empty($_SESSION['event_overview_filter'])) {
        $form['filters']['status']['actions']['reset'] = array(
            '#type' => 'submit',
            '#value' => t('Reset')
        );
    }
    return $form;
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
        form_set_error('', t('No event selected.'));
    }
}

function event_admin_holi_validate($form, &$form_state)
{
    $error = false;
    $err   = "";
    if ($form['options']['name']['#value'] == "") {
        $error = true;
        $err .= "Missing name<br>";
    }
    
    if ($form['options']['operation']['#value'] == "") {
        $error = true;
        $err .= "Missing type<br>";
    }
    
    if ($form['options']['dob']['#value']['date'] == "") {
        $error = true;
        $err .= "Missing date of begining<br>";
        
    }
    
    if ($form['options']['dob2']['#value']['date'] == "") {
        $error = true;
        $err .= "Missing date of ending<br>";
        
    }
    if ($error) {
        form_set_error('', t($err));
    }
}
/**
 * Process event_admin_events form submissions.
 *
 * Execute the chosen 'Update option' on the selected nodes.
 */
function event_admin_events_submit($form, &$form_state)
{
    // Filter out unchecked event nodes.
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
        $obj->event_calendar_status[$field_lang_code][0]['tid'] = $op;
        $node_save                                              = node_save($obj);
    }
    variable_set('node_submitted_check_' . $node_type, 0);
    
    drupal_set_message(t('The status has been changed.'));
}
function event_admin_holi_submit($form, &$form_state)
{
    global $user;
    $td = "";
    if ($form['options']['operation']['#value'] == 1) {
        $td = "important day";
    } else {
        $td = "holiday";
    }
    $nid = db_insert('event_calendar_holiday_custom') // Table name no longer needs {}
        ->fields(array(
        'user_id' => $user->uid,
        'holiday_name' => $form['options']['name']['#value'],
        'holiday_date' => $form['options']['dob']['#value']['date'],
        'holiday_date2' => $form['options']['dob2']['#value']['date'],
        'holiday_type' => $td
    ))->execute();
    
}
function event_calendar_holidays_submit($form, &$form_state)
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
        // Confirmed: processing of the form happens here:
        /*     dpm($form_state); // debug output of $form_state */
        $events = array_filter($_SESSION['our_variable']); // get back the variable (I don't know how $form_state can be preserved from the earlier stage.)
        foreach ($events as $nid) {
            $num_deleted = db_delete('event_calendar_holiday_custom')->condition('holiday_id', $nid)->execute();
        }
        drupal_set_message(t('The Holiday numbers:.' . implode(',', $events) . ' have been deleted'));
        
        
    }
}

function _delete_holiday_nodes()
{
    $node_type = variable_get('event_calendar_node_type', 'event_calendar');
    variable_set('node_submitted_check_' . $node_type, 1);
    $query = db_select('node', 'n');
    $query->join('field_data_event_calendar_status', 'ecs2', 'ecs2.entity_id = n.nid');
    $result = $query->fields('n', array(
        'nid'
    ))->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("important day", "event_calendar_status"))->addTag('')->execute();
    while ($res = $result->fetchAssoc()) {
        
        node_delete($res['nid']);
    }
    
    $query2 = db_select('node', 'n');
    $query2->join('field_data_event_calendar_status', 'ecs2', 'ecs2.entity_id = n.nid');
    $result = $query2->fields('n', array(
        'nid'
    ))->condition('ecs2.event_calendar_status_tid', _get_term_from_name2("holiday", "event_calendar_status"))->addTag('')->execute();
    while ($res = $result->fetchAssoc()) {
        node_delete($res['nid']);
    }
    variable_set('node_submitted_check_' . $node_type, 0);
    drupal_set_message("Deleted all holidays and important days");
}