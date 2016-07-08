<?php

function core_table_cms_admin_types($form, $form_state){
$path         = current_path();
	drupal_add_css(drupal_get_path('module', 'core_table_cms')."/core_table_cms.css");
	
	if (!isset($_SESSION['core_table_cms_admin_delete'])) {
	$form['leave'] = array(
            '#type' => 'link',
			'#title' => t('Go back'),
			'#href' =>'cms/',
            
        );
	$form['types_admin']           = array(
            '#type' => 'fieldset',
            '#title' => t('CMS Types'),
            '#collapsible' => TRUE,
            '#collapsed' => FALSE,
            '#description' => t('Here you can manage your types for CMS core table<br><br>')
        );
		
       $form['types_admin']['add_new'] = array(
            '#type' => 'link',
			'#title' => t('Add new type'),
			'#href' =>'cms/types/new',
            
        );
	$header=array(
		'type_action' => array(
			'data' => t('Type Name')." ",
			'field' => 'n.type_action'
		),
		'original' => array(
			'data' => t('Type')." ",
			'field' => 'n.type_original'
		),
		'records' => array(
			'data' => t("No. Records")." ",
		),
		
		
	);
	
	
	$query = db_select('core_table_cms_type', 'n')->extend('PagerDefault')->extend('TableSort');
        $query->fields('n'); 
		$nids = $query->limit(50)->orderByHeader($header)->addTag('')->execute();
        $options = array();
        while ($node = $nids->fetchAssoc()) {
				$query=db_select('core_table_cms_type', 'n');
				$query->join('core_table_cms', 'n2',"n2.type_action_id=n.type_id");
				$query=$query->fields("n2")->condition("type_action_id",$node['type_id'])->execute();
				$count=0;
				while($query2 = $query->fetchAssoc() ){
					$count++;
				}
            $options[$node['type_id']] = array(
                'type_action' => $node['type_action']." ",
                'original' => (($node['type_original']=="Yes")?"Orignal Type":"Custom Type")." ",
				'records' =>$count." ",
            );
        }
        $form['types_admin']['events'] = array(
            '#type' => 'tableselect',
            '#header' => $header,
            '#options' => $options,
			'#multiple' => FALSE,
			'#default_value' => 2,
            '#empty' => t('No content available.')
        );
        $form['types_admin']['pager']  = array(
            '#markup' => theme('pager')
        );
       
			 $form['types_admin']['delete'] = array(
            '#type' => 'submit',
            '#value' => t('Delete Selected'),
            '#validate' => array(
                'core_cms_events_type_validate'
            ),
            '#submit' => array(
                'core_table_cms_admin_types_submit'
            ));
			$form['types_admin']['edit'] = array(
            '#type' => 'submit',
            '#value' => t('Edit Selected'),
            '#validate' => array(
                'core_cms_events_type_edit_validate'
            ),
			
            '#submit' => array(
                'core_table_cms_admin_types_edit_submit'
            )
            
        );
		}else{//delete confirm

		$do=variable_get("core_table_cms_type_".$_SESSION['core_table_cms_admin_delete']);
			
			 $content = '';
						

   
    $form['intro'] = array('#markup' => $content);


    // create a confirm form:
    // confirm_form($form, $question, $path, $description = NULL, $yes = NULL, $no = NULL, $name = 'confirm');
unset($_SESSION['core_table_cms_admin_delete']);
    return confirm_form($form, $question="Do you really want to delete the type: ".$do['0']['display_name_table'], $path, $description = "Warning, this is the last step before form gets submitted");
	}

	
   
	return $form; 
	}
	function core_cms_events_type_validate($form, &$form_state){
// Error if there are no items to select.
    if ( !isset($form_state['values']['events'])) {
        form_set_error('', t('No selected.'));
	
    }
		$query=db_select('core_table_cms_type', 'n');
	$query=$query->fields("n")->condition("type_id",(int) $form_state['values']['events'])->execute()->fetchAssoc();
	if($query['type_original']=="Yes"){
	form_set_error('', t('Orignal Types cannot be deleted'));
	}
}
	
	
	
	
	
	function core_table_cms_admin_types_edit_submit($form,&$form_state){
	 $_SESSION['core_table_cms_admin_edit']= $form_state['input']['events']; // add our field into a session variable to keep it around for later
     $_SESSION['core_table_cms_admin_edit_first']=true;
	$do=variable_get("core_table_cms_type_".$_SESSION['core_table_cms_admin_edit']);
	$result = db_select($do[0]['table']	, 'c')
		->fields('c')
		->execute()->fetchAssoc();
		if(!isset($result['identifier_cms'])){	  
	 drupal_goto("/cms/types/edit");
	 }else{
	 drupal_set_message("Records found, edit not possible","error");
	 }
    
	
	}
	function core_table_cms_admin_types_submit($form, &$form_state){
	
	  if (!isset($form_state['storage']['confirm'])) {
        // not confirmed yet.
        $_SESSION['core_table_cms_admin_delete']= $form_state['values']['events']; // add our field into a session variable to keep it around for later
$_SESSION['core_table_cms_admin_delete_checker']  = $form_state['values']['events'];     
	   // NOTE: The variable is no longer in $form_state['values']['your-field'] by the time I'm printing out the final confirmation message. I'm
        // not sure session variables are the best solution, but this worked for me.
        $form_state['storage']['confirm'] = TRUE; // this will cause the form to be rebuilt, entering the confirm part of the form
        $form_state['rebuild']            = TRUE;
    }
    
    else {
            
    $_SESSION['core_table_cms_admin_delete']=$_SESSION['core_table_cms_admin_delete_checker'];
	$query=db_select('core_table_cms_type', 'n');
	$query->join('core_table_cms', 'n2',"n2.type_action_id=n.type_id");
	$query=$query->fields("n2")->condition("type_action_id",$_SESSION['core_table_cms_admin_delete'])->execute();
	$count=0;
	while($query2 = $query->fetchAssoc() ){
		$count++;
	}
	if($count!=0){
		drupal_set_message("The type has records assigned to it:".$count."  Cannot delete","error");
	}else{
	$do=variable_get("core_table_cms_type_".$_SESSION['core_table_cms_admin_delete']);
	db_drop_table($do[0]['table']); 
	 db_delete('core_table_cms_type')
	->condition('type_id', $_SESSION['core_table_cms_admin_delete'])
	->execute();
	variable_del("core_table_cms_type_".$_SESSION['core_table_cms_admin_delete']);
	
	}
	unset($_SESSION['core_table_cms_admin_delete']);
	}
	}
	
	
	function core_table_cms_admin_types_new($form, &$form_state){
		
		return core_table_cms_ajax_add_more($form, $form_state);
	
	}
	
	

	
	
	
	
	

function core_table_cms_ajax_add_more($form, &$form_state) {
	$form['leave'] = array(
            '#type' => 'link',
			'#title' => t('Go back'),
			'#href' =>'cms/types',
            
        );
 $form['field_container'] = [
    '#type' => 'container',
    '#weight' => 80,
    '#tree' => TRUE,
    // Set up the wrapper so that AJAX will be able to replace the fieldset.
    '#prefix' => '<div id="js-ajax-elements-wrapper">',
    '#suffix' => '</div>',
  ];
 


  $form_state['field_deltas'] = isset($form_state['field_deltas']) ? $form_state['field_deltas'] : range(1, 3);

  $field_count = $form_state['field_deltas'];
	$options=array("select","textfield","date");
	$options2=array("table","custom");
  foreach ($field_count as $delta) {
    $form['field_container'][$delta] = [
      '#type' => 'container',
      '#attributes' => [
        'class' => ['container-inline'],
      ],
      '#tree' => TRUE,
    ];
 $form['field_container'][$delta]['id'] = [
      '#type' => 'hidden',
      "#value" =>$delta,
      '#size' => 10,
    ];
    $form['field_container'][$delta]['field_name'] = [
      '#type' => 'textfield',
      '#title' => t('Field Name'),
      '#size' => 10,
    ];

	$form['field_container'][$delta]['field_type'] = [
      '#type' => 'select',
	  '#options' => $options,
      '#title' => t('Type'),
     
    ];
	
	$form['field_container'][$delta]['field_type_specify_textfield'] = [
      '#type' => 'textfield',
      '#title' => t("Options**:"),
      '#size' => 10,
    ];
	$form['field_container'][$delta]['field_comment'] = [
      '#type' => 'textfield',
      '#title' => t("Comment:"),
	  '#size' => 10,
      
    ];
	$form['field_container'][$delta]['field_type_required'] = [
      '#type' => 'checkbox',
      '#title' => t("Required"),
      
    ];
	
    $form['field_container'][$delta]['remove_name'] = [
      '#type' => 'submit',
      '#value' => t('-'),
      '#submit' => ['core_table_cms_ajax_add_more_remove'],
      // See the examples in ajax_example.module for more details on the
      // properties of #ajax.
      '#ajax' => [
        'callback' => 'core_table_cms_ajax_add_more_remove_callback',
        'wrapper' => 'js-ajax-elements-wrapper',
      ],
      '#weight' => -50,
      '#attributes' => [
        'class' => ['button-small'],
      ],
      '#name' => 'remove_name_' . $delta,
    ];
  }

  $form['field_container']['add_name'] = [
    '#type' => 'submit',
    '#value' => t('Add more fields'),
    '#submit' => ['core_table_cms_ajax_add_more_add_one'],
    // See the examples in ajax_example.module for more details on the
    // properties of #ajax.
    '#ajax' => [
      'callback' => 'core_table_cms_ajax_add_more_add_one_callback',
      'wrapper' => 'js-ajax-elements-wrapper',
    ],
    '#weight' => 100,
  ];
 $form['field_container']['types_admin']           = array(
            '#type' => 'fieldset',
            '#title' => t('Help'),
            '#description' => t('**Only specify in case it is a select type field, for more options seperate them with "," <br>'),
        '#weight' => 1,
		); 
  $form['other_field']['name'] = [
    '#type' => 'textfield',
    '#title' => t('Type Name'),
	];
	
	
	 $form['other_field']['submission'] = [
      '#type' => 'submit',
            '#value' => t('Create Type'),
            '#validate' => array(
                'core_cms_type_new_validate'
            ),
			'#limit_validation_errors' => array(
      array('field_container'),       // Validate $form_state['values']['step1'].
      array('name'),  // Validate $form_state['values']['foo']['bar'].
    ),
            '#submit' => array(
                'new_type_submit_cms_core'
            ),
			  
	];

  return $form;
}

function core_table_cms_ajax_add_more_remove($form, &$form_state) {
  $delta_remove = $form_state['triggering_element']['#parents'][1];
  $k = array_search($delta_remove, $form_state['field_deltas']);
  unset($form_state['field_deltas'][$k]);

  $form_state['rebuild'] = TRUE;
  drupal_get_messages();
}

function core_table_cms_ajax_add_more_remove_callback($form, &$form_state) {
  return $form['field_container'];
}

function core_table_cms_ajax_add_more_add_one($form, &$form_state) {
  $form_state['field_deltas'][] = count($form_state['field_deltas']) > 0 ? max($form_state['field_deltas']) + 1 : 0;
  
  $form_state['rebuild'] = TRUE;
  drupal_get_messages();
}

function core_table_cms_ajax_add_more_add_one_callback($form, $form_state) {
  return $form['field_container'];
}


function core_cms_type_new_validate($form, &$form_state){
	if($form_state['values']['name']==""){
	form_set_error('name');
	$error=TRUE;

	}
	if(is_numeric(substr($form_state['values']['name'], 0, 1))){
	form_set_error('name');
	$error=TRUE;

	}
	$oga=$form_state['values']['field_container'];
	unset($oga['add_name']);
	$forbit_names=array();
	foreach($oga as $field){
		if($field['field_name']==""){
			form_set_error('field_container]['.$field['id'].'][field_name');
			$error=TRUE;
		}
		if(is_numeric(substr($field['field_name'], 0, 1))){
			form_set_error('field_container]['.$field['id'].'][field_name');
			$error=TRUE;

		}
		foreach($forbit_names as $name){
			if(treatString($field['field_name'])==$name){
				form_set_error('field_container]['.$field['id'].'][field_name');
				$error=TRUE;
			}
		}
		$forbit_names[]=treatString($field['field_name']);
		if($field['field_type']=="0"){
			if($field['field_type_specify_textfield']==""){
				form_set_error('field_container]['.$field['id'].'][field_type_specify_textfield');
				$error=TRUE;
			}
		}

	}
	if(@$error){
		drupal_set_message("There are errors within the submission of the CMS type","error");
	}
}

function new_type_submit_cms_core($form, &$form_state){
 $transaction = db_transaction();
 $error=false;
 $text_error="";
 $oga=$form_state['values']['field_container'];
	unset($oga['add_name']);
	if(count($oga)!=0){
	
	
	try{
	$string_name=treatString($form_state['values']['name']);
	
	if(!db_table_exists("core_table_cms_type_".$string_name)){
		
	
	$nid= db_insert('core_table_cms_type') // Table name no longer needs {}
        ->fields(array(
			'type_original' => 'No',
			'type_action' => $string_name,
		))->execute(); 
		
	
	$query="CREATE TABLE {core_table_cms_type_".$string_name."}( 
	 identifier_cms varchar(255)  NOT NULL,
	";//query for creation of table type
	
	$operis=array();//array for permanent variable
	$operis[0]=array("table" =>"core_table_cms_type_".$string_name, "display_name_table" => $string_name);
	$operis[1]=array("display_name" => "Identifier", "field" =>"identifier_cms", "type" => "hidden", "required" => true);

	$oga=$form_state['values']['field_container'];
	unset($oga['add_name']);
	$w=2;
foreach($oga as $field){
$operis[$w]=array("display_name" =>$field['field_name'], "field" =>treatString($field['field_name']),"comment" =>$field['field_comment']);
			
		
		if($field['field_type']=="0"){
			$query.="`".treatString($field['field_name'])."`"." VARCHAR(255),";
			$operis[$w]["type"]="select";
			$operis[$w]["select_options"]=explode(",",$field['field_type_specify_textfield']);
			
				
		}elseif($field['field_type']=="1"){
		$query.="`".treatString($field['field_name'])."`"." VARCHAR(255),";
			$operis[$w]["type"]="textfield";
			
		}elseif($field['field_type']=="2"){
		$query.="`".treatString($field['field_name'])."`"." DATETIME,";
			$operis[$w]["type"]="date_select";
			$operis[$w]["date_format"] = "Y-m-d H:i:s";
		}
		if($field['field_type_required']==1){
		$operis[$w]["required"]=TRUE;
		}
		$w++;
	}
$query.="PRIMARY KEY (identifier_cms));";
// dpm($operis);
// dpm($query);
variable_set("core_table_cms_type_".$nid,$operis);
db_query($query);
}else{
$error=true;
$text_error="Duplicate type name";
}
}
 catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('my_type', $e);
	$error=true;
	$text_error=$e;
	
  }
  }else{
  $error=true;
	$text_error="No fields in type";
  }
  if($error){
		drupal_set_message($text_error,"error");
	
    
	}else{
		drupal_set_message("Type created");
		
	}
}


function treatString($string) {
    //Lower case everything
    $string = strtolower($string);
    //Make alphanumeric (removes all other characters)
    $string = preg_replace("/[^a-z0-9_\s-]/", "", $string);
   
    //Convert whitespaces and underscore to dash
    $string = preg_replace("/[\s-]/", "_", $string);
    return $string;
}




function core_table_cms_admin_types_edit($form,&$form_state){
$form['field_container'] = [
    '#type' => 'container',
    '#weight' => 80,
    '#tree' => TRUE,
    // Set up the wrapper so that AJAX will be able to replace the fieldset.
    '#prefix' => '<div id="js-ajax-elements-wrapper">',
    '#suffix' => '</div>',
  ];
$do=variable_get("core_table_cms_type_".$_SESSION['core_table_cms_admin_edit']);
		$name=$do[0]['display_name_table'];

		unset($do[0]);
		unset($do[1]);
		$total=count($do);
		$start=1;
		
		if(!isset($form_state['field_deltas'])){
		$form_state['field_deltas']=array();
		while($start<= $total){ 
		$form_state['field_deltas'][] = count($form_state['field_deltas']) > 0 ? max($form_state['field_deltas']) + 1 : 0;
		$start++;
			$first=true;
		}
		}else{
		$form_state['field_deltas']=$form_state['field_deltas'];
		$first=false;
		}
		
  $field_count = $form_state['field_deltas'];
	$options=array("select","textfield","date");
	$options2=array("table","custom");
  foreach ($field_count as $delta) {
    $form['field_container'][$delta] = [
      '#type' => 'container',
      '#attributes' => [
        'class' => ['container-inline'],
      ],
      '#tree' => TRUE,
    ];
 $form['field_container'][$delta]['id'] = [
      '#type' => 'hidden',
      "#value" =>$delta,
      '#size' => 10,
    ];
    $form['field_container'][$delta]['field_name'] = [
      '#type' => 'textfield',
      '#title' => t('Field Name'),
      '#size' => 10,
    ];

	$form['field_container'][$delta]['field_type'] = [
      '#type' => 'select',
	  '#options' => $options,
      '#title' => t('Type'),
     
    ];
	
	$form['field_container'][$delta]['field_type_specify_textfield'] = [
      '#type' => 'textfield',
      '#title' => t("Options**:"),
      '#size' => 10,
    ];
	$form['field_container'][$delta]['field_comment'] = [
      '#type' => 'textfield',
      '#title' => t("Comment:"),
	  '#size' => 10,
      
    ];
	$form['field_container'][$delta]['field_type_required'] = [
      '#type' => 'checkbox',
      '#title' => t("Required"),
      
    ];

  
  

  $form['field_container'][$delta]['remove_name'] = [
      '#type' => 'submit',
      '#value' => t('-'),
      '#submit' => ['core_table_cms_ajax_add_more_remove'],
      // See the examples in ajax_example.module for more details on the
      // properties of #ajax.
      '#ajax' => [
        'callback' => 'core_table_cms_ajax_add_more_remove_callback',
        'wrapper' => 'js-ajax-elements-wrapper',
      ],
      '#weight' => -50,
      '#attributes' => [
        'class' => ['button-small'],
      ],
      '#name' => 'remove_name_' . $delta,
    ];
  }

  $form['field_container']['add_name'] = [
    '#type' => 'submit',
    '#value' => t('Add more fields'),
    '#submit' => ['core_table_cms_ajax_add_more_add_one'],
    // See the examples in ajax_example.module for more details on the
    // properties of #ajax.
    '#ajax' => [
      'callback' => 'core_table_cms_ajax_add_more_add_one_callback',
      'wrapper' => 'js-ajax-elements-wrapper',
    ],
    '#weight' => 100,
  ];
 $form['field_container']['types_admin']           = array(
            '#type' => 'fieldset',
            '#title' => t('Help'),
            '#description' => t('**Only specify in case it is a select type field, for more options seperate them with "," <br>'),
        '#weight' => 1,
		); 
  $form['other_field']['name'] = [
    '#type' => 'textfield',
    '#title' => t('Type Name'),
	];
	
	
	 $form['other_field']['submission'] = [
      '#type' => 'submit',
            '#value' => t('Save Edit'),
            '#validate' => array(
                'core_cms_type_edit_validate'
            ),
			'#limit_validation_errors' => array(
      array('field_container'),       // Validate $form_state['values']['step1'].
      array('name'),  // Validate $form_state['values']['foo']['bar'].
    ),
            '#submit' => array(
                'edit_type_submit_cms_core'
            ),
			  
	];


		
		if($first){
		$form['other_field']['name']['#default_value'] = $name;
    
		$delta=1;
		
		while(isset($form['field_container'][$delta-1])){
		 $form['field_container'][$delta-1]['field_name']['#default_value']=$do[($delta+1)]['display_name'];
	$do[($delta+1)]['type'];
	
	$options=array("select","textfield","date_select");
	while ($fruit_name = current($options)) {
    if ($fruit_name == $do[($delta+1)]['type']) {
    $type=key($options);
    }
    next($options);
	}
	if(isset($type)){
	$form['field_container'][$delta-1]['field_type']['#default_value'] = $type;
		if($type=="select"){$form['field_container'][$delta-1]['field_type_specify_textfield']['#default_value']=(isset($do[($delta+1)]['select_options']))?implode(",",$do[($delta+1)]['select_options']):$do[($delta+1)]['select_table'];}
		$form['field_container'][$delta-1]['field_type_required']['#default_value'] =(isset($do[($delta+1)]['required']))?1:0;
		$form['field_container'][$delta-1]['field_comment']['#default_value']=(isset($do[($delta+1)]['comment']))?$do[($delta+1)]['comment']:'';
	
	}	
	$delta++;
		
	}
	}
	 
	
	return $form;

}




function edit_type_submit_cms_core($form, &$form_state){
 $transaction = db_transaction();
 $error=false;
 $text_error="";
 $oga=$form_state['values']['field_container'];
	unset($oga['add_name']);
	if(count($oga)!=0){
	
	
	try{
	$string_name=treatString($form_state['values']['name']);
	
	
			
			$do=variable_get("core_table_cms_type_".$_SESSION['core_table_cms_admin_edit']);
$result = db_select($do[0]['table']	, 'c')
    ->fields('c')
    ->execute()->fetchAssoc();
	if(!isset($result['identifier_cms'])){
	
	
			
			

	db_drop_table($do[0]['table']);
	$num_deleted = db_delete('core_table_cms_type')
		->condition('type_action', $do[0]['display_name_table'])
		->execute();
		
	$nid= db_insert('core_table_cms_type') // Table name no longer needs {}
        ->fields(array(
			'type_original' => 'No',
			'type_action' => $string_name,
		))->execute(); 
		
	variable_del("core_table_cms_type_".$_SESSION['core_table_cms_admin_edit']);
	$query="CREATE TABLE {core_table_cms_type_".$string_name."}( 
	 identifier_cms varchar(255)  NOT NULL,
	";//query for creation of table type
	
	$operis=array();//array for permanent variable
	$operis[0]=array("table" =>"core_table_cms_type_".$string_name, "display_name_table" => $string_name);
	$operis[1]=array("display_name" => "Identifier", "field" =>"identifier_cms", "type" => "hidden", "required" => true);

	$oga=$form_state['values']['field_container'];
	unset($oga['add_name']);
	$w=2;
foreach($oga as $field){
$operis[$w]=array("display_name" =>$field['field_name'], "field" =>treatString($field['field_name']),"comment" =>$field['field_comment']);
			
		
		if($field['field_type']=="0"){
			$query.=treatString($field['field_name'])." VARCHAR(255),";
			$operis[$w]["type"]="select";
			$operis[$w]["select_options"]=explode(",",$field['field_type_specify_textfield']);
			
				
		}elseif($field['field_type']=="1"){
		$query.=treatString($field['field_name'])." VARCHAR(255),";
			$operis[$w]["type"]="textfield";
			
		}elseif($field['field_type']=="2"){
		$query.=treatString($field['field_name'])." DATETIME,";
			$operis[$w]["type"]="date_select";
			$operis[$w]["date_format"] = "Y-m-d H:i:s";
		}
		if($field['field_type_required']==1){
		$operis[$w]["required"]=TRUE;
		}
		$w++;
	}
$query.="PRIMARY KEY (identifier_cms));";
// dpm($operis);
// dpm($query);
$_SESSION['core_table_cms_admin_edit']=$nid;
variable_set("core_table_cms_type_".$nid,$operis);
db_query($query);

	}else{
	 $error=true;
	$text_error="Records found, edit not possible";
	}
}
 catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('my_type', $e);
	$error=true;
	$text_error=$e;
	
  }
  }else{
  $error=true;
	$text_error="No fields in type";
  }
  if($error){
		drupal_set_message($text_error,"error");
	
    
	}else{
		drupal_set_message("Type edited");
		
	}
	$_SESSION['core_table_cms_admin_edit_first']=True;
	$form_state['rebuild']=True;
}





function core_cms_type_edit_validate($form, &$form_state){
	if($form_state['values']['name']==""){
	form_set_error('name');
	$error=TRUE;

	}
	if(is_numeric(substr($form_state['values']['name'], 0, 1))){
	form_set_error('name');
	$error=TRUE;

	}
	$oga=$form_state['values']['field_container'];
	unset($oga['add_name']);
	$forbit_names=array();
	foreach($oga as $field){
		if($field['field_name']==""){
			form_set_error('field_container]['.$field['id'].'][field_name');
			$error=TRUE;
		}
		if(is_numeric(substr($field['field_name'], 0, 1))){
			form_set_error('field_container]['.$field['id'].'][field_name');
			$error=TRUE;

		}
		foreach($forbit_names as $name){
			if(treatString($field['field_name'])==$name){
				form_set_error('field_container]['.$field['id'].'][field_name');
				$error=TRUE;
			}
		}
		$forbit_names[]=treatString($field['field_name']);
		if($field['field_type']=="0"){
			if($field['field_type_specify_textfield']==""){
				form_set_error('field_container]['.$field['id'].'][field_type_specify_textfield');
				$error=TRUE;
			}
		}

	}
	if(@$error){
		drupal_set_message("There are errors within the submission of the CMS type","error");
	}
}
function core_cms_events_type_edit_validate($form,&$form_state){
if ( !isset($form_state['values']['events'])) {
        form_set_error('', t('No selected.'));
	
    }
		$query=db_select('core_table_cms_type', 'n');
	$query=$query->fields("n")->condition("type_id",(int) $form_state['values']['events'])->execute()->fetchAssoc();
	if($query['type_original']=="Yes"){
	form_set_error('', t('Orignal Types cannot be edited'));
	}
}