# Description:
#   Hubot bindings for the
#   [tiny-doorbell-remote](https://github.com/anroots/tiny-doorbell-remote) project
#
#   Rings the office doorbell when requested.
#
# Dependencies:
#   hubot-auth
#
# Configuration:
#   OFFICE_DOORBELL_API_URL
#   OFFICE_DOORBELL_AUTH
#
# Commands:
#   hubot ding-dong - Tells hubot to ring the office doorbell: there's someone at the door!
#
# Author:
#   anroots

# Full URL to the doorbell API on Raspberry Pi
# Example: http://100.66.4.96:8081
API_URL = process.env.OFFICE_DOORBELL_API_URL

# Base64 HTTP Basic Authorization string
API_AUTH = process.env.OFFICE_DOORBELL_AUTH

module.exports = (robot) ->

  robot.hear /ding-dong/, (res) ->

    # Must be authorized user
    if not robot.auth.hasRole res.envelope.user, 'employee-7th-floor'
      return res.send 'Unauthorized - you should use the actual ' +
        'remote behind the 7th floor door.'

    # Disable proxy
    httpOptions =
      httpAgent: false,
      httpsAgent: false

    robot.logger.info "Posting to #{API_URL}"

    res.http(API_URL, httpOptions)
      .header('Authorization', "Basic #{API_AUTH}")
      .post() (err, postResp, body) ->

        # Network or Apache error (Pi offline, IP changed, not authenticated)
        if postResp.statusCode isnt 200
          robot.logger.error "Doorbell API responded with HTTP #{postResp.statusCode}"
          return res.send 'Could not find the ding-dong button. :/'

        data = JSON.parse body

        # The API reports that GPIO activation failed
        if not data?.status? or data.status isnt 'ok'
          robot.logger.error "Doorbell API responded with an error response: #{body}"
          return res.send 'Pressed the button, but nothing happened. :/'

        # The doorbell rang!
        robot.logger.info 'Ringing the doorbell'
        res.send 'DING DONG! There\'s someone at the door! Is it the pizza guy?!'