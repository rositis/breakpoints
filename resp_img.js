(function ($) {
  var windowWidth = $(window).width();
  Drupal.behaviors.respImg = {
    attach: function (context) {
      if ($.cookie("Drupal.visitor.ri_cookie") === null) {
        var current_suffix = Drupal.settings.respImg.current_suffix;
        if (Drupal.settings.respImg.current_suffix === false) {
          current_suffix = Drupal.settings.respImg.default_suffix;
        }
        
        var suffix = '';
        $.each(Drupal.settings.respImg.suffixes, function(index, value){
          if (value <= screen.availWidth) {
            suffix = index;
            //return false in jQuery.each() equals 'break;' in a traditional loop
            return false;
          }
        });
        
        if (suffix !== '' && suffix !== current_suffix) {
          $('img').each(function() {
            var img = $(this);
            var src = img.attr('src').replace(current_suffix, suffix);
            img.attr('src', src);
          });
          $('a.colorbox').each(function() {
            var a = $(this);
            var href = a.attr('href').replace(current_suffix, suffix);
            a.attr('href', href);
          });
        }
      }

      $.cookie(
        "Drupal.visitor.ri_cookie",
        screen.availWidth,
        {
          path: Drupal.settings.basePath,
          expires: 1
        }
      );
    }
  }
} (jQuery));