(function ($) {
  Drupal.behaviors.respImg = {
    attach: function (context) {
      Drupal.respImg_processSuffixes();
      $(window).resize(function() {
        Drupal.respImg_processSuffixes();
      });

    }
  }
  
  Drupal.respImg_processSuffixes = function() {
    if (true || Drupal.settings.respImg.current_suffix === false || $.cookie("respimg") === null) {
      var current_suffix = Drupal.settings.respImg.current_suffix;
      if (Drupal.settings.respImg.current_suffix === false) {
        current_suffix = Drupal.settings.respImg.default_suffix;
      }
      
      var suffix = '';
      $.each(Drupal.settings.respImg.suffixes, function(index, value){
        if (value <= $(window).width()) {
          suffix = index;
          $.cookie(
            "respimg",
            value,
            {
              path: Drupal.settings.basePath,
              expires: 1
            }
          );
          //return false in jQuery.each() equals 'break;' in a traditional loop
          return false;
        }
      });
      
      if (suffix !== '' && suffix !== current_suffix) {
        $('img').each(function() {
          var img = $(this);
          var src = img.attr('src').replace(current_suffix, suffix);
          img.attr('src', src);
          img.removeAttr('width');
          img.removeAttr('height');
        });
        $('a.colorbox').each(function() {
          var a = $(this);
          var href = a.attr('href').replace(current_suffix, suffix);
          a.attr('href', href);
        });
        Drupal.settings.respImg.current_suffix = suffix;
      }
    }
  }  
} (jQuery));