(function ($) {
  Drupal.behaviors.respImg = {
    attach: function (context) {
      Drupal.respImg_processSuffixes();
      $(window).resize(function() {
        if ($(window).width() > 10) {
          Drupal.respImg_processSuffixes();
        }
      });
    }
  }
  
  Drupal.respImg_getOptimalSuffix = function() {
    var suffix = '';
    $.each(Drupal.settings.respImg.suffixes, function(index, value){
      if (value <= $(window).width()) {
        suffix = index;
        // set cookie with new width
        $.cookie(
          "respimg",
          value,
          {
            path: Drupal.settings.basePath,
            expires: 1
          }
        );
        return false; // exits .each
      }
    });
    return suffix;
  }
  
  Drupal.respImg_processSuffixes = function() {
    // Redirect user if needed / wanted
    if (Drupal.settings.respImg.current_suffix === false && Drupal.settings.respImg.forceRedirect == "1") {
      // Make sure browser accepts cookies
      $.cookie('respimg_test', 'ok');
      if ($.cookie('respimg_test') === 'ok') {
        $.cookie('respimg_test', null);
        var suffix = Drupal.respImg_getOptimalSuffix();
        location.replace(location.href);
      }
    }
    
    // get currently used suffix, or default
    var current_suffix = Drupal.settings.respImg.current_suffix;
    if (Drupal.settings.respImg.current_suffix === false) {
      current_suffix = Drupal.settings.respImg.default_suffix;
    }
    
    // get optimal suffix
    var suffix = Drupal.respImg_getOptimalSuffix();
    
    if (Drupal.settings.respImg.reloadOnResize == "1" && suffix !== '' && suffix !== current_suffix) {
      setTimeout(function() {location.reload(true)}, 100);
      return;
    }
    
    if (Drupal.settings.respImg.forceResize == "1" && suffix !== '' && suffix !== current_suffix) {
      // support for images
      $('img').each(function() {
        var img = $(this);
        var src = img.attr('src').replace(current_suffix, suffix);
        img.attr('src', src);
        img.removeAttr('width');
        img.removeAttr('height');
      });
      
      // support for colorbox links
      $('a.colorbox').each(function() {
        var a = $(this);
        var href = a.attr('href').replace(current_suffix, suffix);
        a.attr('href', href);
      });
      
      // support for field_slideshow (kind of)
      if (typeof(Drupal.behaviors.field_slideshow) == "object") {
        $('div.field-slideshow-processed')
          .cycle('destroy')
          .removeClass('field-slideshow-processed')
          .css('width', '')
          .css('height', '')
          .css('padding-bottom', '')
          .each(function() {
            var $field = $(this);
            var $child = $field.children('div.field-slideshow-slide').first();
            console.log($child);
            console.log($child.css('width'));
            $field.css('width', $child.css('width'));
          });
        $('div.field-slideshow-slide').css('width', '').css('height', '');
        Drupal.behaviors.field_slideshow.attach();
      }
      
      // store last used suffix
      Drupal.settings.respImg.current_suffix = suffix;
    }
  }  
} (jQuery));