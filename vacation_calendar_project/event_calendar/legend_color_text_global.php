 <?php
 
if (module_exists("event_calendar_colors")) {
    $query_one      = db_select('taxonomy_vocabulary', 'ttd')->fields('ttd')->condition('machine_name', 'event_calendar_status', '=');
    $result_one     = $query_one->execute();
    $term_color_div="<div class='full'>";
	$w=1;
    foreach ($result_one as $vid) {
	
        $query  = db_select('taxonomy_term_data', 'ttd')->fields('ttd')->condition('vid', $vid->vid, '=')->orderBy('tid',"ASC");
        $result = $query->execute();
        foreach ($result as $fetch) {
            if($w%2==1){
			 $term_color_div.="<div>";
			}
			$selector                = 'colors_taxonomy_term_' . $fetch->tid;
            $color                   = db_select('event_colors', 'ec')->fields('ec', array(
                'color'
            ))->condition('selector', $selector)->execute()->fetchField();
            $ser_color               = unserialize($color);
            $term_color[$fetch->tid] = $ser_color['background'];
            $term_color_div .= "<div><div style=' display:inline-block;height:20px;width:20px;background-color:" . $ser_color['background'] . "'>&nbsp;&nbsp;&nbsp;&nbsp;</div> - " . $fetch->name;
			if($fetch->tid==_get_term_from_name3("approved", "event_calendar_status")){
			 $term_color_div .=": "._measure_consumed_days2($user->uid, 3);		
			} elseif($fetch->tid==_get_term_from_name3("submitted", "event_calendar_status")){
			 $term_color_div .=": "._measure_consumed_days2($user->uid, 2);		
			}elseif($fetch->tid==_get_term_from_name3("draft", "event_calendar_status")){
			 $term_color_div .=": "._measure_consumed_days2($user->uid, 4);		
			}  
			$term_color_div .= "</div>";
			 if($w%2==0){
			 $term_color_div.="</div>";
			}
        $w++;
		}
    }
    $term_color_div .= "</div>";
    echo $term_color_div;
}
?>