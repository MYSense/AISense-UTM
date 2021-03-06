<?php
/*
 * functions.inc
 *
 * part of pfSense (https://www.pfsense.org)
 * Copyright (c) 2004-2016 Electric Sheep Fencing, LLC
 * All rights reserved.
 *
 * originally part of m0n0wall (http://m0n0.ch/wall)
 * Copyright (c) 2003-2004 Manuel Kasper <mk@neon1.net>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 *    "This product includes software developed by the pfSense Project
 *    for use in the pfSense® software distribution. (http://www.pfsense.org/).
 *
 * 4. The names "pfSense" and "pfSense Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    coreteam@pfsense.org.
 *
 * 5. Products derived from this software may not be called "pfSense"
 *    nor may "pfSense" appear in their names without prior written
 *    permission of the Electric Sheep Fencing, LLC.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *
 * "This product includes software developed by the pfSense Project
 * for use in the pfSense software distribution (http://www.pfsense.org/).
 *
 * THIS SOFTWARE IS PROVIDED BY THE pfSense PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE pfSense PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* BEGIN compatibility goo with HEAD */
if (!function_exists("gettext")) {
	function gettext($text) {
		return $text;
	}
}

if (!function_exists("pfSenseHeader")) {
	/****f* aisense-utils/pfSenseHeader
	 * NAME
	 *   AISenseHeader
	 * INPUTS
	 *   none
	 * RESULT
	 *   Javascript header change or browser Location:
	 ******/
	function pfSenseHeader($text) {
		global $_SERVER;
		if (isAjax()) {
			if ($_SERVER['HTTPS'] == "on") {
				$protocol = "https";
			} else {
				$protocol = "http";
			}

			$port = ":{$_SERVER['SERVER_PORT']}";
			if ($_SERVER['SERVER_PORT'] == "80" && $protocol == "http") {
				$port = "";
			}
			if ($_SERVER['SERVER_PORT'] == "443" && $protocol == "https") {
				$port = "";
			}
			$complete_url = "{$protocol}://{$_SERVER['HTTP_HOST']}{$port}/{$text}";
			echo "\ndocument.location.href = '{$complete_url}';\n";
		} else {
			header("Location: $text");
		}
	}
}
/* END compatibility goo with HEAD */

/*fetch menu notices function*/
if (!function_exists("get_menu_messages")) {
	function get_menu_messages() {
		global $g, $config;
		if (are_notices_pending()) {
			$notices = get_notices();
			$requests = array();

			## Get Query Arguments from URL ###
			foreach ($_REQUEST as $key => $value) {
				if ($key != "PHPSESSID") {
					$requests[] = $key . '=' . $value;
				}
			}
			if (is_array($requests)) {
				$request_string = implode("&", $requests);
			}

			if (is_array($notices)) {
				$notice_msgs = "<table colspan=\'6\' id=\'notice_table\'>";
				$alert_style = "style=\'color:#ffffff; filter:Glow(color=#ff0000, strength=12);\' ";
				$notice = "<a href=\'#\' onclick=notice_action(\'acknowledge\',\'all\');domTT_close(this); {$alert_style}>" . gettext("Acknowledge All Notices") . "</a>";
				$alert_link = "title=\'" . gettext("Click to Acknowledge") . "\' {$alert_style}";
				$domtt_width = 500;
				foreach ($notices as $key => $value) {
					$date = date("m-d-y H:i:s", $key);
					$noticemsg = ($value['notice'] != "" ? $value['notice'] : $value['id']);
					$noticemsg = strip_tags(preg_replace("/(\"|\'|\n|<.?\w+>)/i", "", $noticemsg));
					if ((strlen($noticemsg)* 8) > $domtt_width) {
						$domtt_width = (strlen($noticemsg) *8);
					}
					if ((strlen($noticemsg)* 8) > 900) {
						$domtt_width = 900;
					}
					$alert_action = "onclick=notice_action(\'acknowledge\',\'{$key}\');domTT_close(this);jQuery(this).parent().parent().remove();";
					$notice_msgs .= "<tr><td style=\'vertical-align: top\' width=\'120\'><a href=\'#\' {$alert_link} {$alert_action}>{$date}</a></td><td style=\'vertical-align: top\'><a href=\'#\' {$alert_link} {$alert_action}>[ " . htmlspecialchars($noticemsg) . "]</a></td></tr>";
				}
				$notice_msgs .= "</table>";

				$domtt = "onclick=\"domTT_activate(this, event, 'caption', '{$notice}','content', '<br />{$notice_msgs}', 'trail', false, 'delay', 0, 'fade', 'both', 'fadeMax', 93, 'styleClass', 'niceTitle','width','{$domtt_width}','y',5,'type', 'sticky');\"";
				$menu_messages = "<div id='alerts'>\n";
				if (count($notices) == 1) {
					$msg = sprintf("%1$02d", count($notices)) . " " . gettext("unread notice");
				} else {
					$msg = sprintf("%1$02d", count($notices)) . " " . gettext("unread notices");
				}
				$menu_messages .= "<div id='marquee-text' style='z-index:1001;'><a href='#' {$domtt}><b> .:. {$msg} .:. </b></a></div>\n";
				$menu_messages .= "</div>\n";
			}
		} else {
			$menu_messages = '<div id="hostname">';
			$menu_messages .= $config['system']['hostname'] . "." . $config['system']['domain'];
			$menu_messages .= '</div>';
		}
		return ($menu_messages);
	}
}

if (!function_exists("dom_title")) {
	function dom_title($title_msg, $width=NULL) {
		$width = preg_replace("/\D+/", "", $width);
		if (!empty($width)) {
			$width = ",'width',$width";
		}
		if (!empty($title_msg)) {
			$title_msg = preg_replace("/\s+/", " ", $title_msg);
			$title_msg = preg_replace("/'/", "\'", $title_msg);
			return "onmouseout=\"this.style.color = ''; domTT_mouseout(this, event);\" onmouseover=\"domTT_activate(this, event, 'content', '{$title_msg}', 'trail', true, 'delay', 250, 'fade', 'both', 'fadeMax', 93, 'styleClass', 'niceTitle' $width);\"";
		}
	}
}
/* include all configuration functions */
require_once("interfaces.inc");
require_once("gwlb.inc");
require_once("services.inc");
require_once("aisense-utils.inc");
require_once("certs.inc");
require_once("system.inc");
require_once("vslb.inc");

?>
