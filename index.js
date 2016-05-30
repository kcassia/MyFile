var express = require('express');
var router = express.Router();
var request = require('request');
var $ = require('cheerio')
var async = require('async')
var _ = require('underscore')
var koSpellchecker = require('ko-spellchecker');

koSpellchecker.check = function(strToCheck, cb) {
  async.waterfall([
    function(next) {
      request.post({
        url: 'http://164.125.36.75/PnuSpellerISAPI_201107/lib/PnuSpellerISAPI_201107.dll?Check',
        form: {
          text1: strToCheck
        }
      }, function(err, httpResponse, body) {
        next(err, body)
      })
    },

    function(body, next) {
      var $formBugReport = $('#formBugReport', body)
      var checked_number = $($formBugReport)
          .find('> table')
          .length

      var judged_result = [];

      for (var i = 0; i < checked_number; i++) {
        var error_id = '#tdErrorWord_' + i;
        var replace_id = '#tdReplaceWord_' + i;

        var error_word = $($formBugReport).find(error_id).text();
        var replace_word = $($formBugReport).find(replace_id).text();

        judged_result.push({
          error_word: error_word,
          replace_word: replace_word
        })
      }
      return next(null, judged_result);
    }
  ], function(err, result) {
    cb(err, result)
  })
}

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

router.get('/webhook/', function (req, res) {
  if (req.query['hub.verify_token'] === 'kcassia') {
    res.send(req.query['hub.challenge']);
  }
  res.send('Error, wrong validation token');
});

var messaging_events, event, sender;
router.post('/webhook/', function (req, res) {
  messaging_events = req.body.entry[0].messaging;
  for (i = 0; i < messaging_events.length; i++) {
    event = req.body.entry[0].messaging[i];
    sender = event.sender.id;
    if (event.message && event.message.text) {
      text = event.message.text;
      // Handle a text message from this sender
      
      var str = text.substring(0, 200);
      koSpellchecker.check(text.substring(0, 200), function(err, results){
        if(err)
          return console.error(err)
        for(var i in results) {
          str = str.replace(results[i].error_word, results[i].replace_word);
        }
        sendTextMessage(sender, str);
      });
      
      // sendTextMessage(sender, text.substring(0, 200));
    }
  }
  res.sendStatus(200);
});

var token = "EAADvDGls3OgBAHeRO1PznLExZBB3AX2kNiCJ7cIEBdnaZC7etFZCbykpyZAdPEu6OaQFqUyQygVdYC4vDi9pEmf5uTZAZC1CEYyyUTZCGMeCbFDZB4V4RmzQYkx84DiyZApKxRQc62F2AwEnLnffettSXDtDgVYbLLIRB6UgtyZCaclAZDZD";
var messageData;

function sendTextMessage(sender, text) {
  messageData = {
    text:text
  }
  request({
    url: 'https://graph.facebook.com/v2.6/me/messages',
    qs: {access_token:token},
    method: 'POST',
    json: {
      recipient: {id:sender},
      message: messageData,
    }
  }, function(error, response, body) {
    if (error) {
      console.log('Error sending message: ', error);
    } else if (response.body.error) {
      console.log('Error: ', response.body.error);
    }
  });
}

module.exports = router;

// Local Test
var responseMsg = function(req){
  var res = req;
  koSpellchecker.check(req, function(err, results){
    if(err)
      return console.error(err)
    for(var i in results) {
      res = res.replace(results[i].error_word, results[i].replace_word);
    }
    console.log(res);
  });
  return res;
};
router.get('/msg/:msg', function(req, res){
  res.send('Me : ' + req.params.msg + ', Bot: ' + responseMsg(req.params.msg));
});
