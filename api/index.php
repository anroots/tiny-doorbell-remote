<?php

$cmd = 'sudo /var/doorbell/ring.py';
exec($cmd, $output, $exitCode);

$data = [
    'status' => $exitCode === 0 ? 'ok' : 'failed',
    'output' => implode("\n", $output)
];

header('Content-Type: application/json');
echo json_encode($data);