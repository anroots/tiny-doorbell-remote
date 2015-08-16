# Tiny Doorbell Remote API

These are the components that can be installed on a Raspberry Pi for the purposes of creating a HTTP POST endpoint that triggers an active-low GPIO pin where the trigger of the doorbell remote is connected.

## Installation

You need a Raspberry Pi with Raspbian, PHP, Python, Apache2 and Python GPIO installed.

These are not step-by-step install instructions but guidelines. Some expertise is assumed.

### Virtualhost

Copy `virtualhost.conf` into `/etc/apache2/sites-available/` and enable it with `a2ensite`.

### Listen

If you set the virtualhost to listen on a non-standard HTTP port, edit Apache2's `ports.conf` and add a `Listen` directive.

```
Listen 8081
```

### Web Files

Copy `index.php` and `ring.py` to the Pi. The index file should be in the webroot, this is where HTTP requests come in. The Python file actually activates the GPIO pins.

### Sudo

Allow the python script to execute by adding a `visudo` rule:

```
www-data ALL=NOPASSWD:/var/doorbell/ring.py
```

### Auth

Protect the web endpoint with a `.htpasswd` file.

## Usage

Send a POST request to the index.php endpoint. The script replies with a JSON exit status and hopefully activates the doorbell.