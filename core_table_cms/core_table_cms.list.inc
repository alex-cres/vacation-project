<?php
@include( DRUPAL_ROOT . "/sites/all/modules/event_calendar/event_calendar_holi_counter/event_calendar_holi_counter.list.inc");
/**
 * @file
 * The settings file that allows to add records to the table.
 */


function core_table_cms_content($form, $form_state)
{
    global $user;
	$path         = current_path();
    // Build the 'Create options' form.
	
	if (!isset( $_SESSION['cms_core_table_change'] ) ){
    if (!isset($form_state['storage']['confirm'])) {
    
	if($user->uid!=0){
        $form['event_calendar_custom_ticket']           = array(
            '#type' => 'fieldset',
            '#title' => t('Record Adding'),
            '#collapsible' => TRUE,
            '#collapsed' => FALSE,
            '#description' => t('Record Adding')
        );
		 
    $query     = db_select('core_table_cms_type', 'n');
    $nids      = $query->fields('n')->addTag('')->execute();
    while ($node = $nids->fetchAssoc()) {
        $options[$node['type_id']] = $node['type_action'];
		
		if($options[$node['type_id']]=="vacations"){
			unset($options[$node['type_id']]);//Only possible trough the Vacation Calendar
		}
		
    }
		
		$form['event_calendar_custom_ticket']['operation'] = array(
        '#type' => 'select',
        '#title' => t('Type'),
        '#options' => $options,
        '#default_value' => 'any'
    );
        
        $form['event_calendar_custom_ticket']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Create'),
            '#submit' => array(
                'core_table_adding_submit'
            )
            
        );
		
        
        // Build the sortable table header.
        $header =header_choose();
         $form['filter'] =cms_filter_form_table();
		  $form['#submit'][]= 'core_cms_filter_form_submit';
        $option=options_choose($header);
		 $query     = db_select('core_table_cms_type', 'n');
		$nids      = $query->fields('n')->addTag('')->execute();
		while ($node = $nids->fetchAssoc()) {
			$options[$node['type_id']] = $node['type_action'];
		}
		
		while ($fruit_name = current($options)) {
		
			if(@$_SESSION['core_cms_overview_filter']==key($options)){
				$text_type=t("Table of ".$fruit_name);
				break;
			}else{
				$text_type=t("Table");
			}
			next($options);
		}
		$form['table']=  array(
            '#type' => 'fieldset',
            '#title' => $text_type,
            '#collapsible' => TRUE,
            '#collapsed' => FALSE,
          
        );
        
        $form['table']['events'] = array(
            '#type' => 'tableselect',
            '#header' => $header,
            '#options' => $option,
            '#empty' => t('No content available.')
        );
        $form['table']['pager']  = array(
            '#markup' => theme('pager')
        );
		if(!(@$_SESSION['core_cms_overview_filter']=="1" || @$_SESSION['core_cms_overview_filter']==1)){
		 $form['table']['edit'] = array(
            '#type' => 'submit',
            '#value' => t('Edit Selected'),
            '#validate' => array(
                'core_cms_events_validate'
            ),
            '#submit' => array(
                'change_selected_cms_core'
            )
            
        );
		}
       }else{
		$form['event_calendar_custom_ticket']           = array(
            '#type' => 'fieldset',
            '#title' => t('Access Denied'),
            '#collapsible' => TRUE,
            '#collapsed' => FALSE,
            '#description' => t('Access Denied')
        );
	   }
       }else {
	   
	   $events   = (int) $_SESSION['our_variable'];
       $text_title="";
	    $query     = db_select('core_table_cms_type', 'n');
		$nids      = $query->fields('n')->addTag('')->execute();
		while ($node = $nids->fetchAssoc()) {
			if($node['type_id']==$events){
			$text_title=$node['type_action'];
			}
		}
        
        $form['adding_stuff'] = array(
            '#type' => 'fieldset',
            '#title' => $text_title,
            '#collapsible' => TRUE,
            '#collapsed' => FALSE,
            '#description' => t('Adding '.$text_title)
        );
		$form['adding_stuff']['id'] = array(
				'#type' => 'hidden',
				'#default_value' => $events,
				'#attributes' => array(
					'class' => array(
						'container-inline'
					)
				)
				
			);
			$evarium=variable_get("core_table_cms_type_".$events);
			unset($evarium[0]);
			unset($evarium[1]);
			
			foreach($evarium as $field){
		
			 $form['adding_stuff'][$field['field']] = array(
				'#type' => $field['type'],
				'#title' => t($field['display_name'].":"),
				
				'#attributes' => array(
					'class' => array(
						'container-inline'
					)
				)
			);
			if(isset($field['required'])){
				$form['adding_stuff'][$field['field']]['#required']=TRUE;
			}
			
			if($field['type']=="select"){
			$options=array();

					if(isset($field['select_table'])){
						$query=db_select($field['select_table'],"n")->fields("n",array("identifier_cms"))->orderBy('identifier_cms',$field['order'] )->execute();
					while($op = $query->fetchAssoc()){ 
						$options[]=$op['identifier_cms'];
						}
					}elseif(isset($field['select_options'])){
					$options=$field['select_options'];
					}
				$form['adding_stuff'][$field['field']]['#options']=$options;
			}elseif($field['type']=="date_select"){
				$form['adding_stuff'][$field['field']]['#date_format']=$field['date_format'];
				$form['adding_stuff'][$field['field']]['#default_value']=date("Y-m-d");
				
			}
			if(isset($field['comment'])){
				$form['adding_stuff'][$field['field']]['#description']=$field['comment'];
				}
        }
        $form['adding_stuff']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Create'),
            '#submit' => array(
                'core_table_adding_submit'
            )
            
        );
		
			$form['adding_stuff']['cancel'] = array(
            '#type' => 'submit',
			'#value' => t('Cancel'),
			'#limit_validation_errors' => array(),
            '#submit' => array(
                'core_table_adding_cancel'
            ));
			        
    }
}	
	else{
	$form['change_stuff']=core_cms_change_form_construct();
	$form['change_stuff']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Submit Changes'),
			
            '#submit' => array(
                'change_selected_cms_core'
            )
            
        );
	
		$form['change_stuff']['cancel'] = array(
            '#type' => 'submit',
			'#value' => t('Cancel'),
			'#limit_validation_errors' => array(),
            '#submit' => array(
                'core_table_adding_cancel'
            ));
			
	}
        
    return $form;
}


function core_table_adding_submit($form, &$form_state)
{
    // Filter out unchecked event nodes.
    if (!isset($form_state['storage']['confirm'])) {
        // not confirmed yet.
        $_SESSION['our_variable']         = $form_state['values']['operation']; // add our field into a session variable to keep it around for later
        // NOTE: The variable is no longer in $form_state['values']['your-field'] by the time I'm printing out the final confirmation message. I'm
        // not sure session variables are the best solution, but this worked for me.
        $form_state['storage']['confirm'] = TRUE; // this will cause the form to be rebuilt, entering the confirm part of the form
        $form_state['rebuild']            = TRUE;
    }
    
    else {
      
             
			core_table_cms_submit($form,$form_state);
        
        
    }
}
function core_table_adding_cancel($form, &$form_state)
{
		unset($_SESSION['cms_core_table_change']);
        unset($form_state['storage']['confirm']); 
		$form_state['rebuild']            = TRUE;
}

function core_table_cms_submit($form, $form_state)
{
global $user;
$events  = (int) $form_state['values']['id'];
	$responce=core_table_cms_fill_cms_table($user->uid,$events);
	
	if(isset($responce['request'])){
	drupal_set_message("Saved with '".$responce['request']."'");
	$form_state['values']['identifier_cms']=$responce['request'];
	$evarium=variable_get("core_table_cms_type_".$events);
		
		 $nid     = db_insert($evarium[0]['table']); // Table name no longer needs {}
		 unset($evarium[0]);
		 $array_fields=array('identifier_cms' => $responce['request']);
		  unset($evarium[1]);
foreach($evarium as $field)	{
	if(isset($field['select_options'])){
		$array_fields["`".$field['field']."`"]=$field['select_options'][$form_state['values'][$field['field']]];
	}	else{
	$array_fields["`".$field['field']."`"]=$form_state['values'][$field['field']];
	}
}
		$nid->fields($array_fields)->execute();
		
		
	}
	else{
	drupal_set_message("An error was found".$responce['error_text'],"error");
	}
}



function cms_filter_form_table()
{
    // Collect all terms to show in filter option.
   $query     = db_select('core_table_cms_type', 'n');
    $nids      = $query->fields('n')->addTag('')->execute();
	$options[0]="Any";
    while ($node = $nids->fetchAssoc()) {
        if(module_exists("event_calendar_holi_counter")){
			$options[$node['type_id']] = $node['type_action'];
		}else{
		if($node['type_action']!="vacations"){
			$options[$node['type_id']] = $node['type_action'];
		}
		
		}
    }
    // Creating filter form.
    $form['filters'] = array(
        '#type' => 'fieldset',
        '#title' => t('Show only where'),
        '#theme' => 'exposed_filters__node'
    );
	$form['filters']['status']['filters']           = array(
        '#title' => t('Type'),
        '#type' => 'select',
        '#options' => $options
    );
    if (!empty($_SESSION['core_cms_overview_filter'])) {
        // Find filter status.
        $status                            = db_query("SELECT type_action FROM {core_table_cms_type} WHERE type_id = :tid", array(
            ':tid' => $_SESSION['core_cms_overview_filter']
        ))->fetchField();
        // Extra field to show filter status.
        $form['filters']['status']['text'] = array(
            '#markup' => t('where type is <b> %status </b>', array(
                '%status' => $status
            )),
            '#value' => t('Reset')
        );
    }
    
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
    if (!empty($_SESSION['core_cms_overview_filter'])) {
        $form['filters']['status']['actions']['reset'] = array(
            '#type' => 'submit',
            '#value' => t('Reset')
        );
    }
    return $form;
}

function core_cms_build_filter_query(SelectQueryInterface $query,&$header)
{
    // Build query.
    $filter_data = isset($_SESSION['core_cms_overview_filter']) ? $_SESSION['core_cms_overview_filter'] : '';
    if (isset($_SESSION['core_cms_overview_filter'])) {
        
        $query->condition('n2.type_id', $filter_data);
    }
    
}

function core_cms_filter_form_submit($form, &$form_state)
{
    $filters = $form_state['values']['filters'];
	
    if (!empty($_SESSION['core_cms_overview_filter'])) {
        unset($_SESSION['core_cms_overview_filter']);
    }
    if ($filters!=0) {
        $_SESSION['core_cms_overview_filter'] = $filters;
    }
}

function header_choose(){
	$header=array(
		'identifier' => array(
			'data' => t('Identifier'),
			'field' => 'n.identifier'
		),
		'user_name' => array(
			'data' => t('User'),
			'field' => 'nod.name'
		),
		'date' => array(
			'data' => t('Date'),
			'field' => 'n.date',
			'sort' => 'desc'
		),
		'type_action' => array(
			'data' => t('Type'),
			'field' => 'n.type_action'
		),
	);
	if (!isset($_SESSION['core_cms_overview_filter'])) {
		return $header;
	}elseif($_SESSION['core_cms_overview_filter']==1) {
		if( function_exists('event_calendar_holi_counter_headers')){			
			return event_calendar_holi_counter_headers();
		}else{
			return $header;
		}
	}else{
		return core_table_cms_type_header($_SESSION['core_cms_overview_filter']);
	}
}
function options_choose($header){
	
	if (!isset($_SESSION['core_cms_overview_filter'])) {
		$query = db_select('core_table_cms', 'n')->extend('PagerDefault')->extend('TableSort');
		$query->join('users', 'nod', 'nod.uid = n.user_id');
		$query->join('core_table_cms_type', 'n2', 'n2.type_id = n.type_action_id');
		//TODO:Pass all this inside a function
        $query->fields('n')->fields('nod',array('name'))->fields('n2',array('type_action')); 
		core_cms_build_filter_query($query,$header);
		$nids = $query->limit(50)->orderByHeader($header)->addTag('')->execute();
        $options = array();
        while ($node = $nids->fetchAssoc()) {
			$type="";
            $options[$node['identifier']] = array(
                'identifier' => 'CMS-'.$node['year_part']."-".sprintf('%05d',$node['id_part']),
                'user_name' => $node['name'],
                'date' => $node['date'],
                'type_action' => $node['type_action'],
            );
        }
		return $options;
	}elseif($_SESSION['core_cms_overview_filter']==1) {
		if( function_exists('event_calendar_holi_counter_options')){		
			return event_calendar_holi_counter_options($header);
		}else{
			$query = db_select('core_table_cms', 'n')->extend('PagerDefault')->extend('TableSort');
			$query->join('users', 'nod', 'nod.uid = n.user_id');
			$query->join('core_table_cms_type', 'n2', 'n2.type_id = n.type_action_id');
			//TODO:Pass all this inside a function
			$query->fields('n')->fields('nod',array('name'))->fields('n2',array('type_action')); 
			core_cms_build_filter_query($query,$header);
			$nids = $query->limit(50)->orderByHeader($header)->addTag('')->execute();
			$options = array();
			while ($node = $nids->fetchAssoc()) {
				$type="";
				$options[$node['identifier']] = array(
					'identifier' => 'CMS-'.$node['year_part']."-".sprintf('%05d',$node['id_part']),
					'user_name' => $node['name'],
					'date' => $node['date'],
					'type_action' => $node['type_action'],	
				);
			}
			return $options;
		}
	}else{
		
		return core_table_cms_type_options($header,$_SESSION['core_cms_overview_filter']);

	}
}




function core_table_cms_type_header($w){
	
	$header=array(
		'identifier' => array(
			'data' => t('Identifier'),
			'field' => 'n.identifier'
		),
		'user_name' => array(
			'data' => t('User'),
			'field' => 'nod.name'
		),
		'date' => array(
			'data' => t('Date'),
			'field' => 'n.date',
			'sort' => 'desc'
		),
		
	);
	$nid=variable_get("core_table_cms_type_".$w);
	unset($nid[0]);
		foreach($nid as $field){
		if($field['field']!="identifier_cms"){
			$header[$field['field']]=array(
			'data' =>$field['display_name'],
			
			);
		}
			
			
		
	}
	return $header;
}
function core_table_cms_type_options($header,$w){
	
	$query = db_select('core_table_cms', 'n')->extend('PagerDefault')->extend('TableSort');
		$query->join('users', 'nod', 'nod.uid = n.user_id');
		$query->join('core_table_cms_type', 'n2', 'n2.type_id = n.type_action_id');
		$query->fields('n')->fields('n2')->fields('nod',array('name'))->condition('n.type_action_id',(int) $_SESSION['core_cms_overview_filter']); 
		$nids = $query->limit(50)->orderByHeader($header)->addTag('')->execute();
		$options = array();
		while ($node = $nids->fetchAssoc()) {
			$id_iden= 'CMS-'.$node['year_part']."-".sprintf('%05d',$node['id_part']);
			$nid=variable_get("core_table_cms_type_".$w);
				$query2=db_select($nid[0]['table'], 'n')->fields('n')->condition('n.identifier_cms',$id_iden,'=')->execute();
				$nidsa=$query2->fetchAssoc();
				$options[$node['identifier']] = array(
					'identifier' => $id_iden,
					'user_name' => $node['name'],
					'date' => $node['date'],
					
				);
				unset($nid[0]);
				foreach($nid as $field){
					if($field['field']!="identifier_cms"){
					$options[$node['identifier']][$field['field']]=$nidsa[$field['field']];
					
					}
				}
			}
			
	
		
		return $options;
	
}

function change_selected_cms_core(&$form,&$form_state){
 if (!isset( $_SESSION['cms_core_table_change']  )) {
        // not confirmed yet.
       $_SESSION['cms_core_table_change']      = $form_state['values']['events']; // add our field into a session variable to keep it around for later
        //Values come with identifier except for vacation which comes with the code already
		
		
		
		// NOTE: The variable is no longer in $form_state['values']['your-field'] by the time I'm printing out the final confirmation message. I'm
        // not sure session variables are the best solution, but this worked for me.
        $form_state['storage']['confirm'] = TRUE; // this will cause the form to be rebuilt, entering the confirm part of the form
        $form_state['rebuild']            = TRUE;
        
    } else {
					$recod=array();
	$w=0;
	while(isset($form_state['values'][$w])){
	$nider=variable_get("core_table_cms_type_".$form_state['values']['type'.$w]);
	$num_updated = db_update($nider[0]['table']);
	
	$opter=array();
	unset($nider[0]);
	unset($nider[1]);
	foreach($nider as $field){
	if($field['type']!="select"){
	$opter[$field['field']]=$form_state['values'][$field['field'].$w];
	}else{
	if(isset($field['select_table'])){
				$query3 = db_select($field['select_table'], 'n')->fields("n",array("identifier_cms"))->orderBy('identifier_cms', $field["order"])->execute();
					while($nid=$query3->fetchAssoc()){
						$options[]=$nid['identifier_cms'];
					}
					$opter[$field['field']]= $options[$form_state['values'][$field['field'].$w]];
	}elseif(isset($field['select_options'])){
		$opter[$field['field']]= $field['select_options'][$form_state['values'][$field['field'].$w]];
	}
	}
	}
	
		$num_updated ->fields($opter)->condition('identifier_cms', $form_state['values'][$w],"=")
		->execute();
		

	$recod[]= $form_state['values'][$w];
	$w++;
	
	}
	
	
	drupal_set_message("Updated records: ".implode(",",$recod));
	
	unset($_SESSION['cms_core_table_change']);
	}
}
function core_cms_events_validate($form, &$form_state){
// Error if there are no items to select.
    if (!is_array($form_state['values']['events']) || !count(array_filter($form_state['values']['events']))) {
        form_set_error('', t('No selected.'));
    }
}

function core_cms_change_form_construct(){
	$query = db_select('core_table_cms', 'n')->fields("n")->execute();
	
	while($qu =$query->fetchAssoc()){
	$pas=$_SESSION['cms_core_table_change'];
	$w=0;
		foreach($pas as $ki){
			if($ki==$qu['identifier'] && $qu['type_action_id']!=1){
				$id_iden= 'CMS-'.$qu['year_part']."-".sprintf('%05d',$qu['id_part']);
				$form['edit_'.$qu['identifier']]=array(
				'#type' => 'fieldset',
				'#title' => t('Altering: "'.$id_iden.'"'),
				);
				$form['edit_'.$qu['identifier']][$w]=array(
				'#type' => 'hidden',
				'#value' => $id_iden,
				);
				$form['edit_'.$qu['identifier']]['type'.$w]=array(
				'#type' => 'hidden',
				'#value' => $qu['type_action_id'],
				);
				$nid=variable_get("core_table_cms_type_".$qu['type_action_id']);
	$query2 = db_select($nid[0]['table'], 'n')->fields("n")->condition("n.identifier_cms",$id_iden,"=")->execute()->fetchAssoc();
				unset($nid[0]);
				unset($nid[1]);
				foreach($nid as $field){
		
			 $form['edit_'.$qu['identifier']][$field['field'].$w] = array(
				'#type' => $field['type'],
				'#title' => t($field['display_name'].":"),
				'#default_value' =>$query2[$field['field']],
				'#attributes' => array(
					'class' => array(
						'container-inline'
					)
				)
			);
			if(isset($field['required'])){
				$form['edit_'.$qu['identifier']][$field['field'].$w]['#required']=TRUE;
			}
			if($field['type']=="select"){
			$options=array();

					if(isset($field['select_table'])){
						$query=db_select($field['select_table'],"n")->fields("n",array("identifier_cms"))->orderBy('identifier_cms',$field['order'] )->execute();
					while($op = $query->fetchAssoc()){ 
						$options[]=$op['identifier_cms'];
						}
					}elseif(isset($field['select_options'])){
					$options=$field['select_options'];
					}
				$form['edit_'.$qu['identifier']][$field['field'].$w]['#options']=$options;
			}elseif($field['type']=="date_select"){
				$form['edit_'.$qu['identifier']][$field['field'].$w]['#date_format']=$field['date_format'];
				$form['edit_'.$qu['identifier']][$field['field'].$w]['#default_value']=date("Y-m-d");
			}
        }
				
				
					
			}
			$w++;
		}	
	}
	
	return $form;
}

