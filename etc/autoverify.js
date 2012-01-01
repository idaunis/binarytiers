/**
 * Attaches the autoverify behavior to all required fields
 */
$(document).ready(function() {
    autoverify_init(this);
});

function autoverify_init(context) {
  var avdb = [];
  $('input.autoverify:not(.autoverify-processed)', context).each(function () {
    var uri = this.value;
    
    if (!avdb[uri]) {
      avdb[uri] = new AVDB(uri);
    }
    
    var input = $('#' + this.id.substr(0, this.id.length - 11)).attr('autoverify', 'OFF')[0];
    var status = $('#' + input.id + '-status').attr('autoverify', 'OFF')[0];

    new AV(input, status, avdb[uri]);
    $(this).addClass('autoverify-processed');
  });
}

AV = function (input, status, db) {
  var av = this;
  this.input = input;
  this.status = status;
  this.db = db;

  $(this.input)
    .keyup(function (event) { av.onkeyup(this, event); })
    .blur(function () { av.db.cancel(); });
};

AV.prototype.onkeyup = function (input, e) {
  if (!e) {
    e = window.event;
  }
  switch (e.keyCode) {
    case 16: // shift
    case 17: // ctrl
    case 18: // alt
    case 20: // caps lock
    case 33: // page up
    case 34: // page down
    case 35: // end
    case 36: // home
    case 37: // left arrow
    case 38: // up arrow
    case 39: // right arrow
    case 40: // down arrow
      return true;

    case 9:  // tab
    case 13: // enter
    case 27: // esc
      //this.hidePopup(e.keyCode);
      return true;

    default: // all other keys
      this.populatePopup();
      //else
        //this.hidePopup(e.keyCode);
      return true;
  }
};

AV.prototype.populatePopup = function () {
  this.db.owner = this;
  this.db.search(this.input.value);
};

AV.prototype.setStatus = function (status) {
  switch (status) {
    case 'begin':
      $(this.status).removeClass('found');
      $(this.status).removeClass('notfound');
      break;
    case 'found':
      $(this.status).removeClass('notfound');
      $(this.status).addClass('found');
      break;
    case 'notfound':
      $(this.status).removeClass('found');
      $(this.status).addClass('notfound');
      break;
  }
};


/**
 * An AutoComplete DataBase object
 */
AVDB = function (uri) {
  this.uri = uri;
  this.delay = 100;
  this.cache = {};
};

/**
 * Performs a cached and delayed search
 */
AVDB.prototype.search = function (searchString) {
  var db = this;
  this.searchString = searchString;
  
  // See if this key has been searched for before
  if (this.cache[searchString]) {
    return this.owner.found(this.cache[searchString]);
  }
  // Initiate delayed search
  if (this.timer) {
    clearTimeout(this.timer);
  }
  this.timer = setTimeout(function() {
    //db.owner.setStatus('begin');

    // Ajax GET request for autocompletion
    $.ajax({
      type: "GET",
      url: db.uri +'/'+ encodeURIComponent(searchString),
      dataType: 'json',
      success: function (matches) {
        if (typeof matches['status'] == 'undefined' || matches['status'] != 0) {
          if( matches.found == 1 )
            db.owner.setStatus('found');
          else
            db.owner.setStatus('notfound');
        }
        else db.owner.setStatus('found');
      },
      error: function (xmlhttp) {
        db.owner.setStatus('found');
      }
    });
  }, this.delay);
};

/**
 * Cancels the current autocomplete request
 */
AVDB.prototype.cancel = function() {
  if (this.timer) clearTimeout(this.timer);
  this.searchString = '';
};