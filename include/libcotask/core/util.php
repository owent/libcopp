<?php 
	$max_param_number = 10;
	
function gen_template_params_comment($param_num, $prefix_indent = '', $prefix_msg = '') {
	$ret = '';
	for($i = 0; $i < $param_num; ++$i) {
		$ret .= "$prefix_indent * @param arg$i $prefix_msg parameter $i" . PHP_EOL;
	}
	
	return $ret;
}

function gen_template_params_typename($param_num, $prefix_msg = '') {
	$ret = '';
	for($i = 0; $i < $param_num; ++$i) {
		if (empty($ret))
			$ret = "${prefix_msg}typename TARG$i";
		else
			$ret .= ", typename TARG$i";
	}
	
	return $ret;
}

function gen_template_params_type_param($param_num, $prefix_msg = '', $prefix_vol = '') {
	$ret = '';
	for($i = 0; $i < $param_num; ++$i) {
		if (empty($ret))
			$ret = "${prefix_msg}${prefix_vol}TARG$i arg$i";
		else
			$ret .= ",${prefix_vol} TARG$i arg$i";
	}
	return $ret;
}

function gen_template_params_use_param($param_num, $prefix_msg = '', $prefix_vol = '') {
	$ret = '';
	for($i = 0; $i < $param_num; ++$i) {
		if (empty($ret))
			$ret = "${prefix_msg}${prefix_vol}arg$i";
		else
			$ret .= ",${prefix_vol} arg$i";
	}
	return $ret;
}
