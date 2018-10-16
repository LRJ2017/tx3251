var tnum = 'en';
var docObj;
$(window).resize(function(){
   var h = window.innerWidth - 150;
   $('.translate_wrapper').css("left",h+"px"); 
   //alert(window.innerWidth * 0.15);  
   //alert($('#top-navbar-1').css("left"));    
   $('#top-navbar-1').css("left", window.innerWidth * 0.15 + "px");  
   //alert($('#top-navbar-1').css("left"));  
});

$(document).ready(function(){
  $(document).click( function(e) {
       $('.translate_wrapper, .more_lang').removeClass('active');     
  });
  $('.translate_wrapper .current_lang').click(function(e){    
    e.stopPropagation();
    $(this).parent().toggleClass('active');
    setTimeout(function(){
      $('.more_lang').toggleClass('active');
    }, 5);
  });
  /*TRANSLATE*/
  translate(tnum);
  $('.more_lang .lang').click(function(){
    $(this).addClass('selected').siblings().removeClass('selected');
    $('.more_lang').removeClass('active');  
    
    var img = $(this).find('img').attr('src');    
    var lang = $(this).attr('data-value');
    var tnum = lang;
    translate(tnum);
    $('.current_lang .lang-txt').text(lang);
    $('.current_lang img').attr('src', img);
  });
  
  var h = window.innerWidth - 150;
  $('.translate_wrapper').css("left",h+"px");  
});

function translate(tnum){
	$(document).attr("title",trans_json_other[0][tnum]);
	$('#table-caption').text(trans_json_other[1][tnum]);
	//$('#main_a').find('label').eq(0).text(trans_json[1][tnum]);
	
	//循环为所有label标签赋值
	$('label').each(function(i){
      $(this).text(trans_json_label[i][tnum]);
    });
	//遍历json二维数组trans_json
	//$.each(trans_json,function(key,value) {
	//	console.log(value);
	//	$.each(trans_json[key],function(lang_key,lang_value){
	//		console.log(lang_value);
	//	});
	//});
}

var trans_json_other = [
  { 
    en : 'TX3251 Transport Module Config',
    cn : 'TX3251传输模块配置',
  },
  { 
    en : 'Device state',
    cn : '设备状态',
  },
];


var trans_json_label = [ 
  { 
    en : 'Device state',
    cn : '设备状态',
  },
  { 
    en : 'Controller',
    cn : '控制器设置',
  }, 
  { 
    en : 'Network',
    cn : '网络设置',
  }, 
  { 
    en : 'Water system',
    cn : '水系统配置',
  },
  { 
    en : 'Equipment upgrade',
    cn : '设备升级',
  },
  { 
    en : 'Debugging Log',
    cn : '调试日志',
  },
];
