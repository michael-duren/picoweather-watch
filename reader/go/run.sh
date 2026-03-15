#!/bin/bash
set -e

# Cleanup on exit
trap 'kill 0' EXIT

# Clean and create Grafana directories
GRAFANA_PROVISIONING_DIR="/tmp/grafana-provisioning"
rm -rf "$GRAFANA_PROVISIONING_DIR"
rm -rf /tmp/grafana-data
rm -rf /tmp/grafana-logs
rm -rf /tmp/grafana-plugins

mkdir -p "$GRAFANA_PROVISIONING_DIR/datasources"
mkdir -p "$GRAFANA_PROVISIONING_DIR/dashboards"
mkdir -p "$GRAFANA_PROVISIONING_DIR/dashboards/configs"
mkdir -p "$GRAFANA_PROVISIONING_DIR/plugins"
mkdir -p "$GRAFANA_PROVISIONING_DIR/alerting"
mkdir -p /tmp/grafana-data
mkdir -p /tmp/grafana-logs
mkdir -p /tmp/grafana-plugins

# Copy datasource configuration to provisioning directory
cp "../grafana-datasource.yml" "$GRAFANA_PROVISIONING_DIR/datasources/"

# Copy dashboard provisioning configuration
cp "../grafana-dashboard-provisioning.yml" "$GRAFANA_PROVISIONING_DIR/dashboards/"

# Copy dashboard JSON to configs subdirectory
cp "../grafana-dashboard.json" "$GRAFANA_PROVISIONING_DIR/dashboards/configs/"

echo "Starting services..."

# Start Go application
go run main.go &
GO_PID=$!

# Start Prometheus
prometheus --config.file="../prometheus.yml" &
PROMETHEUS_PID=$!

# Start Grafana with custom config
grafana server \
  --homepath="/opt/homebrew/share/grafana" \
  --config="../grafana.ini" &
GRAFANA_PID=$!

echo "Services started:"
echo "  - Go app (PID: $GO_PID) - http://localhost:8080/metrics"
echo "  - Prometheus (PID: $PROMETHEUS_PID) - http://localhost:9090"
echo "  - Grafana (PID: $GRAFANA_PID) - http://localhost:3000 (admin/admin)"
echo ""
echo "Press Ctrl+C to stop all services"

wait
