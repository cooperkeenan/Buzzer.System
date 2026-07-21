/**
 * Twilio Function - inbound SMS door unlock
 *
 * Deployed at path /sms with visibility set to PROTECTED, so Twilio rejects any
 * request without a valid X-Twilio-Signature. That is what stops anyone who
 * finds the URL from opening the door - do not set this function to Public.
 *
 * Replaces the ESP32's old /sms endpoint. Rather than reaching into the home
 * network, this validates the code and writes an unlock request to Firebase,
 * which the device collects by polling. No port forwarding, no public IP.
 *
 * Environment variables (Twilio console > Functions > Environment Variables):
 *   FIREBASE_HOST   e.g. buzzer-system-9d674-default-rtdb.firebaseio.com
 *   FIREBASE_SECRET database secret, appended as ?auth=
 *
 * Uses only the Node standard library so it needs no dependency configuration.
 */

const https = require('https');

const CODE_PATH = '/doorAccess/currentCode.json';
const UNLOCK_REQUEST_PATH = '/doorAccess/unlockRequest.json';

const SUPPORT_NUMBER = '07967770146';

function firebaseRequest(context, path, method, body) {
  const auth = context.FIREBASE_SECRET ? `?auth=${context.FIREBASE_SECRET}` : '';

  const options = {
    hostname: context.FIREBASE_HOST,
    path: `${path}${auth}`,
    method,
    headers: body ? { 'Content-Type': 'application/json' } : {},
  };

  return new Promise((resolve, reject) => {
    const req = https.request(options, (res) => {
      let payload = '';
      res.on('data', (chunk) => { payload += chunk; });
      res.on('end', () => {
        if (res.statusCode < 200 || res.statusCode >= 300) {
          reject(new Error(`Firebase ${method} ${path} returned ${res.statusCode}`));
          return;
        }
        resolve(payload);
      });
    });

    req.on('error', reject);
    if (body) req.write(body);
    req.end();
  });
}

function normalize(value) {
  return String(value == null ? '' : value).trim().toUpperCase();
}

exports.handler = async function (context, event, callback) {
  const twiml = new Twilio.twiml.MessagingResponse();
  const reply = (message) => {
    twiml.message(message);
    callback(null, twiml);
  };

  const submittedCode = normalize(event.Body);

  if (!submittedCode) {
    return reply(`❌ Error: No message body received.\nContact support: ${SUPPORT_NUMBER}`);
  }

  let validCode;
  try {
    const payload = await firebaseRequest(context, CODE_PATH, 'GET');
    validCode = normalize(JSON.parse(payload));
  } catch (error) {
    console.error('Failed to fetch access code:', error.message);
    return reply(`❌ System error: Cannot reach database. Please try again later or contact support.\n${SUPPORT_NUMBER}`);
  }

  if (!validCode) {
    console.error('No access code configured in Firebase');
    return reply(`❌ System error: No valid code configured. Contact support.\n${SUPPORT_NUMBER}`);
  }

  if (submittedCode !== validCode) {
    console.log(`Rejected code from ${event.From}`);
    return reply(`❌ Invalid code. Please check and try again.\nFor help: ${SUPPORT_NUMBER}`);
  }

  // Timestamp in seconds so the ESP32 can hold it in a 32-bit unsigned long.
  // The device refuses anything older than its freshness window.
  const unlockRequest = {
    id: `${Date.now()}-${Math.random().toString(36).slice(2, 10)}`,
    status: 'pending',
    requestedAt: Math.floor(Date.now() / 1000),
  };

  try {
    await firebaseRequest(context, UNLOCK_REQUEST_PATH, 'PUT', JSON.stringify(unlockRequest));
  } catch (error) {
    console.error('Failed to raise unlock request:', error.message);
    return reply(`❌ System error: Cannot reach database. Please try again later or contact support.\n${SUPPORT_NUMBER}`);
  }

  console.log(`Unlock request ${unlockRequest.id} raised for ${event.From}`);
  return reply('✅ Door unlocked! Welcome home!');
};
